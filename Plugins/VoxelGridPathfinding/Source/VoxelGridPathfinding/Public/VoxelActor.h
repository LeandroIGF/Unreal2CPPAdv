// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Components/BoxComponent.h"
#include "Components/HierarchicalInstancedStaticMeshComponent.h"
#include "Engine/World.h"
#include "VoxelGridSubsystem.h" // Per accedere all'enum EVoxelState

#include "VoxelActor.generated.h"

// Nota: EVoxelState e FVoxelData ora sono in VoxelGridSubsystem.h

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
	
	// Voxel Grid CONFIGURATION (Now in Project Settings)


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

	// API FUNCTIONS WAPPERS
	UFUNCTION(BlueprintCallable, Category = "Voxel Grid")
	void RequestRegionAsyn(FVector Center, float Radius);

private:

	// Reference to Subsystem
	UPROPERTY()
	UVoxelGridSubsystem* VoxelSubsystem;

	// Function bound to Subsystem Event
	UFUNCTION()
	void OnVoxelUpdated(const FIntVector& Coord, EVoxelState NewState);

	UFUNCTION()
	void OnPathFound(const TArray<FVector>& PathPoints);

};
