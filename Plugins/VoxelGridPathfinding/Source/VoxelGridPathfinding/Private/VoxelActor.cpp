
#include "VoxelActor.h"
#include "VoxelGridSubsystem.h"
//#include "VoxelTickableSubsystem.h"
#include "Engine/World.h"
#include "DrawDebugHelpers.h"

// Sets default values
AVoxelActor::AVoxelActor()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false; // No more tick needed for logic!

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
}

// Called when the game starts or when spawned
void AVoxelActor::BeginPlay()
{
	Super::BeginPlay();
	
	// Get Subsystem
	VoxelSubsystem = GetWorld()->GetSubsystem<UVoxelGridSubsystem>();

	if (VoxelSubsystem)
	{
		// VoxelSubsystem->InitializeGrid(VoxelSize, MaxRequestsPerTick); // Config is auto via settings now

		// Bind to updates
		VoxelSubsystem->OnVoxelUpdated.AddDynamic(this, &AVoxelActor::OnVoxelUpdated);
		VoxelSubsystem->OnOnPathFound.AddDynamic(this, &AVoxelActor::OnPathFound);
	}
}

// Called every frame
void AVoxelActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	// Logic moved to Subsystem
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

void AVoxelActor::RequestRegionAsyn(FVector Center, float Radius)
{
	if (VoxelSubsystem)
	{
		VoxelSubsystem->RequestRegionAsyn(Center, Radius);
	}
}

void AVoxelActor::OnVoxelUpdated(const FIntVector& Coord, EVoxelState NewState)
{
	if (!VoxelSubsystem) return;

	FVector WorldPos = VoxelSubsystem->GridToWorld(Coord);
	float CurrentVoxelSize = VoxelSubsystem->GetVoxelSize();

	if (NewState == EVoxelState::Blocked)
	{
		// Calculate scale based on ue5 base cube: 100x100x100
		float ScaleFactor = CurrentVoxelSize / 100.f;

		FTransform InstanceTransform;
		InstanceTransform.SetLocation(WorldPos);
		InstanceTransform.SetScale3D(FVector(ScaleFactor));

		//Add instance to HISM
		//HISM_Block->AddInstance(InstanceTransform);
		
		// Debug Visuals
		if(VoxelSubsystem->bShowVoxelDebug)
		{
			DrawDebugBox(GetWorld(), WorldPos, FVector(CurrentVoxelSize / 2.f), FColor::Red, true, 20.f); // Increased duration
		}
	}
	else if (NewState == EVoxelState::Free)
	{
		// Just debug for free space
		if (VoxelSubsystem->bShowVoxelDebug)
		{
			DrawDebugBox(GetWorld(), WorldPos, FVector(CurrentVoxelSize / 2.f), FColor::Green, true, 20.f);
		}
	}
}

void AVoxelActor::OnPathFound(const TArray<FVector>& PathPoints)
{

	if (!VoxelSubsystem || !VoxelSubsystem->bShowPathfindingDebug) return;

	if (PathPoints.IsEmpty()) return;

	// Draw Path
	for (int32 i = 0; i < PathPoints.Num() - 1; i++)
	{
		
		DrawDebugLine(GetWorld(), PathPoints[i], PathPoints[i + 1], FColor::Cyan, false, 10.f, 0, 5.0f);				
		DrawDebugSphere(GetWorld(), PathPoints[i], 10.f, 8, FColor::Yellow, false, 10.f);
	}

	DrawDebugSphere(GetWorld(), PathPoints[PathPoints.Num() - 1], 10.f, 8, FColor::Yellow, false, 10.f);
}