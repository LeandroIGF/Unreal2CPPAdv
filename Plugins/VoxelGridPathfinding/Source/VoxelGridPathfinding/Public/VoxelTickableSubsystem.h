// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/WorldSubsystem.h"
#include "VoxelTickableSubsystem.generated.h"

// Enum e Struct Refactor

UENUM(BlueprintType)
enum class EVoxelStateTickable : uint8
{
	Unknown,	// Not Initialized
	Free,		// Free to move
	Blocked		// Obstacle
};

// Struct for voxel data

USTRUCT(BlueprintType)
struct FVoxelDataTickable
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadOnly)
	EVoxelStateTickable State = EVoxelStateTickable::Unknown;
};

// struct for A* nodes

struct FPathNodeTick
{
	FIntVector Coord;								// Grid Coordinate
	FIntVector ParentCoord;							// Parent Coordinate
	float GCost;									// Cost from start to current node
	float HCost;									// Heuristic cost to target
	float FCost() const { return GCost + HCost; }	// Total cost

	// Constructors for struct
	FPathNodeTick() : GCost(0), HCost(0) {}

	FPathNodeTick(FIntVector InCoord, FIntVector InParent, float InGCost, float InHCost) :
		Coord(InCoord), ParentCoord(InParent), GCost(InGCost), HCost(InHCost) {}

	// Operator overload for comparison <
	bool operator<(const FPathNodeTick& Other) const
	{
		return FCost() < Other.FCost();
	}
};

// Delegate per la notifica degli aggiornamenti
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnVoxelTickUpdated, const FIntVector&, Coord, EVoxelStateTickable, NewState);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnPathFound, const TArray<FVector>&, PathPoints);

/**
 * 
 * Gestisce la logica del Voxel Grid: dati, generazione asincrona e stati dei singoli voxel
 * Gestisce il pathfinding: A* implementazione semplice
 * 
 */
UCLASS()
class VOXELGRIDPATHFINDING_API UVoxelTickableSubsystem : public UTickableWorldSubsystem
{
	GENERATED_BODY()

public:

	UVoxelTickableSubsystem();

	//Subsystem Interface to implement
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;			// Inizializzazione
	virtual void Deinitialize() override;											// Deinizializzazione
	virtual void Tick(float DeltaTime) override;									// Funzione Tick perchè implementa interfaccia tickable
	virtual bool IsTickable() const override { return true; }						// Check if object can tick
	virtual TStatId GetStatId() const override										// Need to override correctly or the engine crash
	{
		// We create a new CYCLE STAT for the trace system
		RETURN_QUICK_DECLARE_CYCLE_STAT(UVoxelTickableSubsystem, STATGROUP_Tickables);
	}
	//End Subsystem interface 

	// API FUNCTIONS
	UFUNCTION(BlueprintCallable, Category = "Voxel Grid Tickable")
	void RequestRegionAsynTick(FVector Center, float Radius);

	// Helpers
	FIntVector WorldToGridTick(FVector WorldPosition) const;
	FVector GridToWorldTick(FIntVector GridPos) const;

	// Only read Grid 
	const TMap<FIntVector, FVoxelDataTickable>& GetVoxelGrid() const { return VoxelGrid; }
	float GetVoxelSize() const { return VoxelSize; }

	// Debug flags
	bool bShowVoxelDebug = true;
	bool bShowPathfindingDebug = true;

	// Delegate
	UPROPERTY(BlueprintAssignable, Category = "Voxel Grid")
	FOnVoxelTickUpdated OnVoxelTickUpdated;

	// PATHFINDING API

	UFUNCTION(BlueprintCallable, Category = "Voxel Pathfinding")
	void FindPathAsyncTick(FVector StartWorldLocation, FVector TargetWorldLocation);

private:

	// config data
	float VoxelSize = 100.f;
	int32 MaxRequestsPerTick = 5;

	// Internal Data

	// Thread Safe (access from GameThread)
	TMap<FIntVector, FVoxelDataTickable> VoxelGrid;

	// Coda per la fisica
	TArray<FIntVector> GenerationQueue;

	// Mappa per associare le richieste asyn (Handle) alle coordinate
	TMap<FTraceHandle, FIntVector> PendingTraceHandle;

	// Delegate for physic traces results
	FOverlapDelegate OverlapDelegate;

	// Delegate for pathfinding results
	FOnPathFound OnPathFoundDelegate;

	// Logic
	void ProcessQueueTick();
	void OnTraceCompletedTick(const FTraceHandle& Handle, FOverlapDatum& OverlapData);
	void PerformAStarSearchTick(FIntVector StartGrid, FIntVector EndGrid, float LocalVoxelSize, FVector StartWorld, FVector EndWorld);

};
