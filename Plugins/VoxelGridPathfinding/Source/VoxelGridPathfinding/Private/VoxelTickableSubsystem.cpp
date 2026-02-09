// Fill out your copyright notice in the Description page of Project Settings.


#include "VoxelTickableSubsystem.h"
#include "VoxelGridSettings.h"

UVoxelTickableSubsystem::UVoxelTickableSubsystem()
{
}

void UVoxelTickableSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);

	// Load Settings and override internal config variables

	const UVoxelGridSettings* Settings = GetDefault<UVoxelGridSettings>();

	if (IsValid(Settings))
	{
		VoxelSize = Settings->VoxelSize;
		MaxRequestsPerTick = Settings->MaxRequestsPerTick;

		MaxIterInternal = Settings->MaxIters;

		bShowVoxelDebug = Settings->bShowVoxelDebug;
		bShowPathfindingDebug = Settings->bShowPathfindingDebug;
	}

	//Bind del delagate della risposta asincrona
	OverlapDelegate.BindUObject(this, &UVoxelTickableSubsystem::OnTraceCompletedTick);
}

void UVoxelTickableSubsystem::Deinitialize()
{
	Super::Deinitialize();
}

void UVoxelTickableSubsystem::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	// Simulation Work

	if (GenerationQueue.Num() > 0)
	{
		ProcessQueueTick();
	}
}

void UVoxelTickableSubsystem::RequestRegionAsynTick(FVector Center, float Radius)
{
	float LocalVoxelSize = VoxelSize;

	Async(EAsyncExecution::ThreadPool,
		[this, Center, Radius, LocalVoxelSize]()
		{
			//We are in the thread now
			TArray<FIntVector> VoxelPositions;
			int32 GridRadius = FMath::CeilToInt(Radius / LocalVoxelSize);

			// Calculate voxel positions within the radius
			int32 SideLenght = GridRadius * 2;
			VoxelPositions.Reserve(SideLenght * SideLenght * SideLenght);

			// Center in grid coordinates
			FIntVector CenterGrid = WorldToGridTick(Center);

			// Iteration For each voxel in the cube volume
			for (int32 x = -GridRadius; x <= GridRadius; x++)
			{
				for (int32 y = -GridRadius; y <= GridRadius; y++)
				{
					for (int32 z = -GridRadius; z <= GridRadius; z++)
					{
						if ((x * x + y * y * +z * z) <= (GridRadius * GridRadius))
						{
							VoxelPositions.Add(CenterGrid + FIntVector(x, y, z));
						}
					}
				}
			}

			// We go back to the game thread to enqueue the work
			AsyncTask(ENamedThreads::GameThread,
				[this, VoxelPositions]()
				{
					for (const FIntVector& Coord : VoxelPositions)
					{
						// TODO: Better check which voxels are new and which are already present in the queue
						if (!VoxelGrid.Contains(Coord))
						{
							GenerationQueue.AddUnique(Coord);
						}
					}
				}
			);
		}
	);
}





FIntVector UVoxelTickableSubsystem::WorldToGridTick(FVector WorldPosition) const
{
	return FIntVector(
		FMath::FloorToInt(WorldPosition.X / VoxelSize),
		FMath::FloorToInt(WorldPosition.Y / VoxelSize),
		FMath::FloorToInt(WorldPosition.Z / VoxelSize)
	);
}

FVector UVoxelTickableSubsystem::GridToWorldTick(FIntVector GridPos) const
{
	return FVector(
		(GridPos.X * VoxelSize) + (VoxelSize / 2.f),
		(GridPos.Y * VoxelSize) + (VoxelSize / 2.f),
		(GridPos.Z * VoxelSize) + (VoxelSize / 2.f)
	);
}

void UVoxelTickableSubsystem::ProcessQueueTick()
{
	int32 SentCount = 0;

	while (GenerationQueue.Num() > 0 && SentCount < MaxRequestsPerTick)
	{
		FIntVector CurrentCoord = GenerationQueue.Pop();

		// Skip if already present in the grid
		if (VoxelGrid.Contains(CurrentCoord)) continue;

		FVector WorldPos = GridToWorldTick(CurrentCoord);

		// Setup Async Physics Trace
		FCollisionQueryParams QueryParams;
		QueryParams.bTraceComplex = false;

		// We make a smaller box for the trace to avoid edge cases
		FCollisionShape BoxShape = FCollisionShape::MakeBox(FVector(VoxelSize / 2.f) * 0.9f);

		// Start the async overlap trace
		FTraceHandle TraceHandle = GetWorld()->AsyncOverlapByChannel(
			WorldPos,										// Location to trace
			FQuat::Identity,								// Rotation (no rotation for box)
			ECollisionChannel::ECC_WorldStatic,				// Collision Channel
			BoxShape,										// Collision Shape
			QueryParams,									// Query Params
			FCollisionResponseParams::DefaultResponseParam,	// Response Params
			&OverlapDelegate								// Delegate for completion
		);

		//We add the trace handle to the pending map
		PendingTraceHandle.Add(TraceHandle, CurrentCoord);
		SentCount++;
	}
}

void UVoxelTickableSubsystem::OnTraceCompletedTick(const FTraceHandle& Handle, FOverlapDatum& OverlapData)
{
	if (FIntVector* FoundCoord = PendingTraceHandle.Find(Handle))
	{
		FIntVector Coord = *FoundCoord;
		PendingTraceHandle.Remove(Handle);

		bool bHit = OverlapData.OutOverlaps.Num() > 0;

		// Update the voxel data
		FVoxelDataTickable VoxelInfo;

		if (bHit)
		{
			VoxelInfo.State = EVoxelStateTickable::Blocked;
		}
		else
		{
			VoxelInfo.State = EVoxelStateTickable::Free;
		}

		// Aggiungi alla mappa
		VoxelGrid.Add(Coord, VoxelInfo);

		// Notifica chi e' in ascolto (es. il Visualizzatore)
		OnVoxelTickUpdated.Broadcast(Coord, VoxelInfo.State);
	}
}

// PATHFINDING ASYNC TICKABLE METHODS

void UVoxelTickableSubsystem::FindPathAsyncTick(FVector StartWorldLocation, FVector TargetWorldLocation)
{
	FIntVector StartGrid = WorldToGridTick(StartWorldLocation);
	FIntVector EndGrid = WorldToGridTick(TargetWorldLocation);
	float LocalVoxelSize = VoxelSize;

	Async(EAsyncExecution::ThreadPool,
		[this, StartGrid, EndGrid, LocalVoxelSize, StartWorldLocation, TargetWorldLocation]()
		{
			PerformAStarSearchTick(StartGrid, EndGrid, LocalVoxelSize, StartWorldLocation, TargetWorldLocation);
		}
	);


}

void UVoxelTickableSubsystem::PerformAStarSearchTick(FIntVector StartGrid, FIntVector EndGrid, float LocalVoxelSize, FVector StartWorld, FVector EndWorld)
{
	UE_LOG(LogTemp, Warning, TEXT("[A*] Start Pathfinding from %s to %s in grid, %s to %s in world"), 
		*StartGrid.ToString(),
		*EndGrid.ToString(),
		*StartWorld.ToString(),
		*EndWorld.ToString());

	// A* Algorithm Implementation

	TArray<FVector> PathPoints; // Resulting path points in world coordinates
	TMap<FIntVector, FPathNodeTick> OpenSet; // Nodes to be evaluated
	TSet<FIntVector> ClosedSet; // Nodes already evaluated

	// Map to track
	TMap<FIntVector, FIntVector> CameFrom; // For path reconstruction
	TMap<FIntVector, float> GScore; // Cost from start to current node

	// Check start/end validity
	bool bStartValid = false;
	
	bStartValid = VoxelGrid.Contains(StartGrid) && VoxelGrid[StartGrid].State == EVoxelStateTickable::Free;

	if (!bStartValid)
	{
		UE_LOG(LogTemp, Error, TEXT("[A*] Start Pathfinding Invalid or Blocked: %s grid, %s world. Aborting."), 
			*StartGrid.ToString(),
			*StartWorld.ToString());

		// Verify if inside the grid
		if (!VoxelGrid.Contains(StartGrid))
		{
			UE_LOG(LogTemp, Error, TEXT("[A*] Start Position is NOT in the voxel grid!"));
		}
		else
		{
			UE_LOG(LogTemp, Error, TEXT("[A*] Start Position is BLOCKED!"));
		}

		// Return with broadcast
		AsyncTask(ENamedThreads::GameThread,
			[this, PathPoints]()
			{
				OnPathFoundDelegate.Broadcast(PathPoints);
			}
		);
		return;
	}

	// Setup Node for start
	FPathNodeTick StartNode(StartGrid, FIntVector::ZeroValue, 0, FVector::Dist(FVector(StartGrid), FVector(EndGrid)));
	OpenSet.Add(StartGrid, StartNode);
	CameFrom.Add(StartGrid, FIntVector::ZeroValue);
	GScore.Add(StartGrid, 0.f);

	// Directions for neighbor nodes (6-connected grid)
	FIntVector Directions[6] = {
		FIntVector(1, 0, 0),FIntVector(-1, 0, 0),
		FIntVector(0, 1, 0),FIntVector(0, -1, 0),
		FIntVector(0, 0, 1),FIntVector(0, 0, -1)
	};

	bool bFound = false;
	//int32 MaxIter = 10000;
	int32 IterCount = 0;

	while (OpenSet.Num() > 0 && IterCount < MaxIterInternal)
	{
		IterCount++;

		// Get node with lowest F cost
		float LowestF = FLT_MAX;
		FIntVector CurrentCoord = FIntVector::ZeroValue;

		for (const auto& Pair : OpenSet)
		{
			if (Pair.Value.FCost() < LowestF)
			{
				LowestF = Pair.Value.FCost();
				CurrentCoord = Pair.Key;
			}
		}

		if(CurrentCoord == EndGrid)
		{
			bFound = true;
			UE_LOG(LogTemp, Log, TEXT("[A*] Target Reached at iter: %d"), IterCount);
			break;
		}

		OpenSet.Remove(CurrentCoord);

		// Debug every 500 iterations
		if(IterCount % 500 == 0)
		{
			UE_LOG(LogTemp, Log, TEXT("[A*] Iter: %d , OpenSet: %d , Current: %s"), IterCount, OpenSet.Num(), *CurrentCoord.ToString());
		}

		// Explore neighbors
		for (const FIntVector& Dir : Directions)
		{
			FIntVector NextCoord = CurrentCoord + Dir;

			// Check if walkable
			bool bWalkable = false;
			if (const FVoxelDataTickable* V = VoxelGrid.Find(NextCoord))
			{
				if (V->State == EVoxelStateTickable::Free)
				{
					bWalkable = true;
				}
			}

			// Check Poison / Water

			if (!bWalkable) continue;

			float NewCost = GScore[CurrentCoord] + 1; // Distance 1 for neighbors

			if (!GScore.Contains(NextCoord) || NewCost < GScore[NextCoord])
			{
				GScore.Add(NextCoord, NewCost);
				float Priority = NewCost + FVector::Dist(FVector(NextCoord), FVector(EndGrid));
				OpenSet.Add(NextCoord, FPathNodeTick(NextCoord, CurrentCoord, NewCost, Priority));
				CameFrom.Add(NextCoord, CurrentCoord);
			}
		}	
	}

	// PATH FOUND? 
	if (bFound)
	{
		UE_LOG(LogTemp, Warning, TEXT("[A*] PATH FOUND! Starting Reconstruction..."));

		FIntVector CurrentGridPosition = EndGrid;

		while (CurrentGridPosition != StartGrid)
		{
			FVector WorldVector = GridToWorldTick(CurrentGridPosition);

			PathPoints.Add(WorldVector);

			if (CameFrom.Contains(CurrentGridPosition))
			{
				CurrentGridPosition = CameFrom[CurrentGridPosition];
			}
			else
			{
				UE_LOG(LogTemp, Warning, TEXT("[A*] ERROR in Path Reconstruction"));
				break;
			}
		}

		PathPoints.Add(GridToWorldTick(StartGrid));

		// Reverse
		Algo::Reverse(PathPoints);
		UE_LOG(LogTemp, Warning, TEXT("[A*] PathPoints: %d"), PathPoints.Num());
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("[A*] FAILED TO FIND THE PATH"));
		UE_LOG(LogTemp, Error, TEXT("[A*] OpenSet Empty: %s, MaxIter Reached: %s"),
			(OpenSet.Num() == 0 ? TEXT("Yes") : TEXT("No")),
			(IterCount >= MaxIterInternal ? TEXT("Yes") : TEXT("No")));
	}

	// FINISHED!, BACK TO GAME THREAD
	AsyncTask(ENamedThreads::GameThread,
		[this, PathPoints]()
		{
			OnPathFoundDelegate.Broadcast(PathPoints);
		}
	);
}







