// Fill out your copyright notice in the Description page of Project Settings.

#pragma once


#include "CoreMinimal.h"
#include "Subsystems/WorldSubsystem.h"
#include "VoxelGridSubsystem.generated.h"

// Enum e Struct spostati qui per essere accessibili globalmente
UENUM(BlueprintType)
enum class EVoxelState : uint8
{
	Unknown,	// Not Initialized
	Free,		// Free to move
	Blocked		// Obstacle
};

USTRUCT(BlueprintType)
struct FVoxelData
{
	GENERATED_BODY()
	
	UPROPERTY(BlueprintReadOnly)
	EVoxelState State = EVoxelState::Unknown;
};

// Delegato per notificare gli aggiornamenti
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnVoxelUpdated, const FIntVector&, Coord, EVoxelState, NewState);

/**
 * Gestisce la logica centrale della Voxel Grid: dati, generazione asincrona e stato.
 */
UCLASS()
class VOXELGRIDPATHFINDING_API UVoxelGridSubsystem : public UTickableWorldSubsystem
{
	GENERATED_BODY()

public:
	// Sets default values for this subsystem's properties
	UVoxelGridSubsystem();

	virtual void Initialize(FSubsystemCollectionBase& Collection) override;
	virtual void Deinitialize() override;
	virtual void Tick(float DeltaTime) override;
	virtual bool IsTickable() const override { return true; }
	virtual TStatId GetStatId() const override
	{
		RETURN_QUICK_DECLARE_CYCLE_STAT(UVoxelGridSubsystem, STATGROUP_Tickables);
	}

	// --- CONFIGURATION ---
	// Configuration is handled via Project Settings (UVoxelGridSettings)


	// --- API FUNCTIONS ---
    
	UFUNCTION(BlueprintCallable, Category = "Voxel Grid")
	void RequestRegionAsyn(FVector Center, float Radius, FVector BoxExtent);

	// --- HELPERS ---
	FIntVector WorldToGrid(FVector WorldPosition) const;
	FVector GridToWorld(FIntVector GridPos) const;
	int32 GetNextIdRequest();
	
	// Accesso sola lettura alla griglia
	// Accesso sola lettura alla griglia
	const TMap<FIntVector, FVoxelData>& GetVoxelGrid() const { return VoxelGrid; }
	float GetVoxelSize() const { return VoxelSize; }

	// --- DEBUG FLAGS (Synced from Settings) ---
	bool bShowVoxelDebug = true;
	bool bShowPathfindingDebug = true;

	// --- EVENTS ---
	
	// Delegato per restituire il percorso trovato (in coordinate World)
	DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnPathFound, const TArray<FVector>&, PathPoints, const int32, IDRequest);

	UPROPERTY(BlueprintAssignable, Category = "Voxel Grid")
	FOnPathFound OnOnPathFound; // Note: Typo in original request fix if needed, standard is OnPathFound. I will use OnPathFound.

	UPROPERTY(BlueprintAssignable, Category = "Voxel Grid")
	FOnVoxelUpdated OnVoxelUpdated;


	
	// --- PATHFINDING ---

	TArray<int32> IDRequests;

	UFUNCTION(BlueprintCallable, Category = "Voxel Pathfinding")
	void FindPathAsync(FVector StartWorld, FVector EndWorld, int32 IDRequest);

private:

	// --- CONFIG DATA ---
	float VoxelSize = 100.f; 
	int32 MaxRequestsPerTick = 5;

	// --- INTERNAL DATA ---
	
	// Thread Safe (access from GameThread)
	TMap<FIntVector, FVoxelData> VoxelGrid;

	// Coda di lavoro per la fisica
	TArray<FIntVector> GenerationQueue;

	// Mappa per associare le richieste async (Handle) alle coordinate
	TMap<FTraceHandle, FIntVector> PendingTraceHandle;

	// Delegato per gestire i risultati delle tracce
	FOverlapDelegate OverlapDelegate;

	// --- LOGIC ---
	void ProcessQueue();
	void OnTraceCompleted(const FTraceHandle& Handle, FOverlapDatum& OverlapData);
	void PerformAStarSearch(FIntVector StartGrid, FIntVector EndGrid, float LocalVoxelSize, FVector StartWorld, int32 IDRequest);
};
