// Fill out your copyright notice in the Description page of Project Settings.


#include "VoxelActor.h"

// Sets default values
AVoxelActor::AVoxelActor()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	// 1. Setup Volume Component
	Volume = CreateDefaultSubobject<UBoxComponent>(TEXT("Volume"));
	RootComponent = Volume;
	Volume->SetBoxExtent(FVector(5000.f, 5000.f, 1000.f));
	Volume->SetCollisionProfileName(TEXT("NoCollision"));

	// 2. SETUP HISM COMPONENT
	HISM_Block = CreateDefaultSubobject<UHierarchicalInstancedStaticMeshComponent>(TEXT("HISMComponent"));
	HISM_Block->SetupAttachment(RootComponent);
	HISM_Block->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	HISM_Block->SetCastShadow(false);
	HISM_Block->SetCanEverAffectNavigation(false);

	// 3. Bind del delegate per la risposta asincrona

	OverlapDelegate.BindUObject(this, &AVoxelActor::OnTraceCompleted);


}

// Called when the game starts or when spawned
void AVoxelActor::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AVoxelActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	// Simulation work
	if(GenerationQueue.Num() > 0)
	{
		ProcessQueue();
	}
}

void AVoxelActor::OnConstruction(const FTransform& Transform)
{
	Super::OnConstruction(Transform);

	if(ObstacleMesh)
	{
		HISM_Block->SetStaticMesh(ObstacleMesh);
	}

	if (ObstacleMaterial)
	{
		HISM_Block->SetMaterial(0, ObstacleMaterial);
	}
}

// 1. MATH CALCULATIONS
// Request a voxel region around a center point with a given radius
// Asynchronous version
void AVoxelActor::RequestRegionAsyn(FVector Center, float Radius)
{
	// Copy the values to pass to the thread in a safe way

	float LocalVoxelSize = VoxelSize;

	Async(EAsyncExecution::ThreadPool,
		[this, Center, Radius, LocalVoxelSize]()
		{
			//We are in the thread now
			TArray<FIntVector> VoxelPositions;

			int32 GridRadius = FMath::CeilToInt(Radius / LocalVoxelSize);

			// Calculate voxel positions within the radius
			int32 SideLenght = GridRadius * 2;

			// Reserve memory for the volume (Cube Volume)
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

// 2 DISPATCHING AND PROCESSING
void AVoxelActor::ProcessQueue()
{
	int32 SentCount = 0;

	while (GenerationQueue.Num() > 0 && SentCount < MaxRequestsPerTick)
	{
		FIntVector CurrentCoord = GenerationQueue.Pop();
		
		// Skip if already present in the grid
		if (VoxelGrid.Contains(CurrentCoord)) continue;

		FVector WorldPos = GridToWorld(CurrentCoord);

		// Bounds Check if inside the volume
		FVector LocalPos = GetActorTransform().InverseTransformPosition(WorldPos);
		FBox Bounds = FBox(-Volume->GetScaledBoxExtent(), Volume->GetScaledBoxExtent());

		if (!Bounds.IsInside(LocalPos)) continue;

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

// 3. RESULT AND RENDERING
void AVoxelActor::OnTraceCompleted(const FTraceHandle& Handle, FOverlapDatum& OverlapData)
{
	if (FIntVector* FoundCoord = PendingTraceHandle.Find(Handle))
	{
		FIntVector Coord = *FoundCoord;
		PendingTraceHandle.Remove(Handle);

		bool bHit = OverlapData.OutOverlaps.Num() > 0;

		// Update the voxel data
		FVoxelData VoxelInfo;

		// For now we render only if there is a hit
		// TODO: Improve visualization
		if (bHit)
		{
			VoxelInfo.State = EVoxelState::Blocked;

			FVector WorldPos = GridToWorld(Coord);

			// Calculate scale based on ue5 base cube: 100x100x100
			// If VoxelSize is 200, scale should be 2

			float ScaleFactor = VoxelSize / 100.f;

			FTransform InstanceTransform;
			InstanceTransform.SetLocation(WorldPos);
			InstanceTransform.SetScale3D(FVector(ScaleFactor));

			//Add instance to HISM
			//HISM_Block->AddInstance(InstanceTransform);
			DrawDebugBox(GetWorld(), WorldPos, FVector(VoxelSize / 2.f), FColor::Red, true, 10.f);

		}
		else
		{
			VoxelInfo.State = EVoxelState::Free;

			FVector WorldPos = GridToWorld(Coord);

			DrawDebugBox(GetWorld(), WorldPos, FVector(VoxelSize / 2.f), FColor::Green, true, 10.f);
		}
	}
}

FIntVector AVoxelActor::WorldToGrid(FVector WorldPosition) const
{
	// Convert world position to grid coordinates based on voxel size
	return FIntVector(
		FMath::FloorToInt(WorldPosition.X / VoxelSize),
		FMath::FloorToInt(WorldPosition.Y / VoxelSize),
		FMath::FloorToInt(WorldPosition.Z / VoxelSize)
	);
}

FVector AVoxelActor::GridToWorld(FIntVector GridPos) const
{
	// Convert grid coordinates back to world position based on voxel size
	return FVector(
		(GridPos.X * VoxelSize) + (VoxelSize / 2.f),
		(GridPos.Y * VoxelSize) + (VoxelSize / 2.f),
		(GridPos.Z * VoxelSize) + (VoxelSize / 2.f)
	);
}