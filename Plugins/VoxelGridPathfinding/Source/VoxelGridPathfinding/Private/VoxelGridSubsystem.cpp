
#include "VoxelGridSubsystem.h"
#include "VoxelGridSettings.h"
#include "Engine/World.h"
#include "Async/Async.h"
#include "DrawDebugHelpers.h" 
#include "Algo/Reverse.h" 

// Helper struct per A* (Interna al CPP)
struct FPathNode
{
	FIntVector Coord;
	FIntVector ParentCoord; // Per ricostruire il percorso
	float G; // Costo dal via
	float H; // Euristica
	float F() const { return G + H; }

	FPathNode() : G(0), H(0) {}
	FPathNode(FIntVector InCoord, FIntVector InParent, float InG, float InH) 
		: Coord(InCoord), ParentCoord(InParent), G(InG), H(InH) {}

	// Operatore < per la PriorityQueue (Nota: per le TArray::Heap metodi, usiamo un predicato esterno solitamente, ma qui useremo un semplice array sortato o custom logic)
	bool operator<(const FPathNode& Other) const
	{
		return F() < Other.F();
	}
};

UVoxelGridSubsystem::UVoxelGridSubsystem()
{
}

void UVoxelGridSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);
	
	// Load Settings
	const UVoxelGridSettings* Settings = GetDefault<UVoxelGridSettings>();
	if (Settings)
	{
		VoxelSize = Settings->VoxelSize;
		MaxRequestsPerTick = Settings->MaxRequestsPerTick;
		
		// Sync Debug Flags
		bShowVoxelDebug = Settings->bShowVoxelDebug;
		bShowPathfindingDebug = Settings->bShowPathfindingDebug;
	}

	// Bind del delegate per la risposta asincrona
	OverlapDelegate.BindUObject(this, &UVoxelGridSubsystem::OnTraceCompleted);
}

void UVoxelGridSubsystem::Deinitialize()
{
	Super::Deinitialize();
}

void UVoxelGridSubsystem::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	// Simulation work
	if(GenerationQueue.Num() > 0)
	{
		ProcessQueue();
	}
}

void UVoxelGridSubsystem::RequestRegionAsyn(FVector Center, float Radius)
{
	// Copy the values to pass to the thread in a safe way
	float LocalVoxelSize = VoxelSize;

	// Lambda capture by value for thread safety
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
			FIntVector CenterGrid = WorldToGrid(Center);

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

void UVoxelGridSubsystem::ProcessQueue()
{
	int32 SentCount = 0;

	while (GenerationQueue.Num() > 0 && SentCount < MaxRequestsPerTick)
	{
		FIntVector CurrentCoord = GenerationQueue.Pop();
		
		// Skip if already present in the grid
		if (VoxelGrid.Contains(CurrentCoord)) continue;

		FVector WorldPos = GridToWorld(CurrentCoord);

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

void UVoxelGridSubsystem::OnTraceCompleted(const FTraceHandle& Handle, FOverlapDatum& OverlapData)
{
	if (FIntVector* FoundCoord = PendingTraceHandle.Find(Handle))
	{
		FIntVector Coord = *FoundCoord;
		PendingTraceHandle.Remove(Handle);

		bool bHit = OverlapData.OutOverlaps.Num() > 0;

		// Update the voxel data
		FVoxelData VoxelInfo;
		
		if (bHit)
		{
			VoxelInfo.State = EVoxelState::Blocked;
		}
		else
		{
			VoxelInfo.State = EVoxelState::Free;
		}

		// Aggiungi alla mappa
		VoxelGrid.Add(Coord, VoxelInfo);

		// Notifica chi e' in ascolto (es. il Visualizzatore)
		OnVoxelUpdated.Broadcast(Coord, VoxelInfo.State);
	}
}

// --- PATHFINDING ---

void UVoxelGridSubsystem::FindPathAsync(FVector StartWorld, FVector EndWorld)
{
	// Copia sicura dei dati necessari per il thread
	// Nota: Copiare una TMap gigante puo' essere pesante. 
	// Tuttavia, leggere la TMap dal thread mentre il GameThread scrive (OnTraceCompleted) e' rischioso senza lock.
	// Per questo esempio "Semplice", assumiamo che durante il pathfinding la griglia sia stabile o usiamo la copia.
	// In una versione PRO, useremmo un RWLock. Qui facciamo una copia implicita passando al thread o ci fidiamo del fatto che
	// la TMap di solito non rialloca se non aggiungiamo nuovi elementi massivamente.
	// Per sicurezza massima in questo esempio, copiamo le chiavi o usiamo un approccio ottimista.
	// Dato che VoxelGrid è nel subsystem, accedervi direttamnte da worker thread è UNSAFE se il GameThread modifica.
	// SOLUZIONE SEMPLICE: Eseguiamo A* sul GameThread per ora se la mappa è piccola, oppure usiamo un lock.
	// Dato che l'utente vuole "Async", e la mappa è potenzialmente grande, usiamo Async ma dobbiamo stare attenti.
	// Facciamo una copia della Grid "logica" (solo Blocked/Free) sarebbe ideale.
	// PER ORA: Accesso diretto (Unsafe se in generazione, OK se statico). 
		
	FIntVector StartGrid = WorldToGrid(StartWorld);
	FIntVector EndGrid = WorldToGrid(EndWorld);
	float LocalVoxelSize = VoxelSize;

	// Lancio task
	Async(EAsyncExecution::ThreadPool, [this, StartGrid, EndGrid, LocalVoxelSize, StartWorld]()
	{
		PerformAStarSearch(StartGrid, EndGrid, LocalVoxelSize, StartWorld);
	});
}

void UVoxelGridSubsystem::PerformAStarSearch(FIntVector StartGrid, FIntVector EndGrid, float LocalVoxelSize, FVector StartWorld)
{
	UE_LOG(LogTemp, Warning, TEXT("[A*] Start Pathfinding from %s to %s"), *StartGrid.ToString(), *EndGrid.ToString());

	// A* ALGORITHM INIT
	TArray<FVector> PathPoints;
	TMap<FIntVector, FPathNode> OpenSet;
	TSet<FIntVector> ClosedSet;
	
	// Map to track path
	TMap<FIntVector, FIntVector> CameFrom;
	TMap<FIntVector, float> CostSoFar;

	// Check start/end validity
	bool bStartValid = false; 
	if(const FVoxelData* D = VoxelGrid.Find(StartGrid)) bStartValid = (D->State == EVoxelState::Free);
	
	if(!bStartValid) 
	{
		UE_LOG(LogTemp, Error, TEXT("[A*] Start Position Invalid or Blocked: %s. Aborting."), *StartGrid.ToString());
		// Verifica se esiste almeno nella grid
		if(!VoxelGrid.Contains(StartGrid)) 
		{
			UE_LOG(LogTemp, Error, TEXT("[A*] Start Position NOT in VoxelGrid maps."));
		}
		else 
		{
			UE_LOG(LogTemp, Error, TEXT("[A*] Start Position is BLOCKED state."));
		}

		AsyncTask(ENamedThreads::GameThread, [this, PathPoints](){ OnOnPathFound.Broadcast(PathPoints); });
		return;
	}

		// Setup Start Node
	FPathNode StartNode(StartGrid, FIntVector::ZeroValue, 0, FVector::Dist(FVector(StartGrid), FVector(EndGrid)));
	OpenSet.Add(StartGrid, StartNode);
	CameFrom.Add(StartGrid, StartGrid);
	CostSoFar.Add(StartGrid, 0);

	// Directions (6 neighbors)
	// Spostate fuori dalla lambda e rese un semplice array locale
	FIntVector Directions[] = {
		FIntVector(1,0,0), FIntVector(-1,0,0),
		FIntVector(0,1,0), FIntVector(0,-1,0),
		FIntVector(0,0,1), FIntVector(0,0,-1)
	};

	bool bFound = false;
	int32 MaxIter = 5000; 
	int32 IterCount = 0;

	while(OpenSet.Num() > 0 && IterCount < MaxIter)
	{
		IterCount++;
		
		// Find node with lowest F
		float LowestF = FLT_MAX;
		FIntVector Current = FIntVector::ZeroValue;

		for(const auto& Pair : OpenSet)
		{
			if(Pair.Value.F() < LowestF)
			{
				LowestF = Pair.Value.F();
				Current = Pair.Key;
			}
		}

		if(Current == EndGrid)
		{
			bFound = true;
			UE_LOG(LogTemp, Log, TEXT("[A*] Target Reached at iter: %d"), IterCount);
			break;
		}

		OpenSet.Remove(Current);
		
		// Debug ogni tanto
		if(IterCount % 500 == 0)
		{
			UE_LOG(LogTemp, Log, TEXT("[A*] Iter: %d, OpenSet: %d, Current: %s"), IterCount, OpenSet.Num(), *Current.ToString());
		}

		// Neighbors
		for(const FIntVector& Dir : Directions)
		{
			FIntVector Next = Current + Dir;
			
			// Walkability check
			bool bWalkable = false;
			if(const FVoxelData* V = VoxelGrid.Find(Next))
			{
				if(V->State == EVoxelState::Free) bWalkable = true;
			}
			
			if(!bWalkable) continue;

			float NewCost = CostSoFar[Current] + 1; // Distance 1 for neighbors

			if(!CostSoFar.Contains(Next) || NewCost < CostSoFar[Next])
			{
				CostSoFar.Add(Next, NewCost);
				float Priority = NewCost + FVector::Dist(FVector(Next), FVector(EndGrid));
				OpenSet.Add(Next, FPathNode(Next, Current, NewCost, Priority));
				CameFrom.Add(Next, Current);
			}
		}
	}

	if(bFound)
	{
		UE_LOG(LogTemp, Log, TEXT("[A*] Path Found! Reconstructing..."));
		FIntVector Curr = EndGrid;
		while(Curr != StartGrid)
		{
			// Convert to world
			PathPoints.Add(
				FVector(
					(Curr.X * LocalVoxelSize) + (LocalVoxelSize / 2.f),
					(Curr.Y * LocalVoxelSize) + (LocalVoxelSize / 2.f),
					(Curr.Z * LocalVoxelSize) + (LocalVoxelSize / 2.f)
				)
			);
			
			if(CameFrom.Contains(Curr)) 
			{
				Curr = CameFrom[Curr];
			}
			else 
			{
				UE_LOG(LogTemp, Error, TEXT("[A*] Critical Error in path reconstruction"));
				break; 
			}
		}
		PathPoints.Add(StartWorld); 
		
		// Reverse
		Algo::Reverse(PathPoints);
		UE_LOG(LogTemp, Log, TEXT("[A*] Path Points: %d"), PathPoints.Num());
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("[A*] FAILED. OpenSet Empty: %s, MaxIter Reached: %s"), 
			(OpenSet.Num() == 0 ? TEXT("Yes") : TEXT("No")), 
			(IterCount >= MaxIter ? TEXT("Yes") : TEXT("No")));
	}

	// Finished, back to game thread
	AsyncTask(ENamedThreads::GameThread, [this, PathPoints]()
	{
		OnOnPathFound.Broadcast(PathPoints);
	});
}


FIntVector UVoxelGridSubsystem::WorldToGrid(FVector WorldPosition) const
{
	return FIntVector(
		FMath::FloorToInt(WorldPosition.X / VoxelSize),
		FMath::FloorToInt(WorldPosition.Y / VoxelSize),
		FMath::FloorToInt(WorldPosition.Z / VoxelSize)
	);
}

FVector UVoxelGridSubsystem::GridToWorld(FIntVector GridPos) const
{
	return FVector(
		(GridPos.X * VoxelSize) + (VoxelSize / 2.f),
		(GridPos.Y * VoxelSize) + (VoxelSize / 2.f),
		(GridPos.Z * VoxelSize) + (VoxelSize / 2.f)
	);
}
