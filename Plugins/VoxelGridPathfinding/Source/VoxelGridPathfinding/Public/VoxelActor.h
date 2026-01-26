// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Components/BoxComponent.h"
#include "Components/HierarchicalInstancedStaticMeshComponent.h"
#include "Engine/World.h"
#include "VoxelActor.generated.h"

enum class EVoxelState : uint8
{
	Unknown,	// Not Initialized
	Free,		// Free to move
	Blocked		// Obstacle
};

struct FVoxelData
{
	EVoxelState State = EVoxelState::Unknown;
};

UCLASS()
class VOXELGRIDPATHFINDING_API AVoxelActor : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AVoxelActor();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	virtual void OnConstruction(const FTransform& Transform) override;
	
	// Voxel Grid CONFIGURATION
	UPROPERTY(EditAnywhere, Category = "Grid Config")
	float VoxelSize = 100.f; // Dimension cube size

	UPROPERTY(EditAnywhere, Category = "Grid Config")
	int32 MaxRequestsPerTick = 5; // Max pathfinding requests to process per tick

	// Mesh to visualize
	UPROPERTY(EditAnywhere, Category = "Visualization")
	TObjectPtr<UStaticMesh> ObstacleMesh;

	// Material for obstacles
	UPROPERTY(EditAnywhere, Category = "Visualization")
	TObjectPtr<UMaterialInterface> ObstacleMaterial;

	// COMPONENTS
	UPROPERTY(EditAnywhere, Category = "Components")
	TObjectPtr<UBoxComponent> Volume;

	UPROPERTY(EditAnywhere, Category = "Components")
	TObjectPtr<UHierarchicalInstancedStaticMeshComponent> HISM_Block;

	// API FUNCTIONS

	UFUNCTION(BlueprintCallable, Category = "Voxel Grid")
	void RequestRegionAsyn(FVector Center, float Radius);


	// Helpers

	FIntVector WorldToGrid(FVector WorldPosition) const;
	FVector GridToWorld(FIntVector GridPos) const;

private:

	//Internal Voxel Grid Data

	// Thread Safe in lettura e scrittura dal nostro Game Thread
	TMap<FIntVector, FVoxelData> VoxelGrid;

	// Coda di lavoro per la fisica
	TArray<FIntVector> GenerationQueue;

	// Mappa per associare le richieste async (Handle) alle coordinate
	TMap<FTraceHandle, FIntVector> PendingTraceHandle;

	// Delegato per gestire i risultati delle tracce
	FOverlapDelegate OverlapDelegate;

	// LOGIC FUNCTIONS

	void ProcessQueue();
	void OnTraceCompleted(const FTraceHandle& Handle, FOverlapDatum& OverlapData);

};
