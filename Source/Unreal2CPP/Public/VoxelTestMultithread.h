#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Components/BoxComponent.h"
#include "Components/HierarchicalInstancedStaticMeshComponent.h"
#include "Engine/World.h"
#include "VoxelTestMultithread.generated.h"

// Stati possibili del voxel
enum class EVoxelState : uint8
{
    Unknown,
    Free,
    Blocked
};

// Dati minimi salvati in memoria
struct FVoxelData
{
    EVoxelState State = EVoxelState::Unknown;
};

UCLASS()
class UNREAL2CPP_API AVoxelTestMultithread : public AActor
{
    GENERATED_BODY()

public:
    AVoxelTestMultithread();
    virtual void Tick(float DeltaTime) override;
    virtual void OnConstruction(const FTransform& Transform) override; // Per aggiornare mesh in editor

    // --- CONFIGURAZIONE ---

    UPROPERTY(EditAnywhere, Category = "Grid Config")
    float VoxelSize = 100.0f; // Dimensione del cubo (es. 100cm)

    UPROPERTY(EditAnywhere, Category = "Grid Config")
    int32 MaxPhysicsRequestsPerFrame = 500; // Bilancia questo numero in base alla CPU

    // Mesh da usare per visualizzare gli ostacoli (es. Shape_Cube)
    UPROPERTY(EditAnywhere, Category = "Visualization")
    UStaticMesh* ObstacleMesh;

    // Materiale per gli ostacoli (es. Rosso)
    UPROPERTY(EditAnywhere, Category = "Visualization")
    UMaterialInterface* ObstacleMaterial;

    // --- COMPONENTI ---

    UPROPERTY(VisibleAnywhere, Category = "Components")
    UBoxComponent* BoundsVolume;

    // Componente ottimizzato per disegnare milioni di istanze
    UPROPERTY(VisibleAnywhere, Category = "Components")
    UHierarchicalInstancedStaticMeshComponent* HISM_Blocked;

    // --- API ---

    /** Calcola regione in background. Radius alto (es. 9000) supportato. */
    UFUNCTION(BlueprintCallable, Category = "Grid")
    void RequestRegionAsync(FVector Center, float Radius);

    // Helpers
    FIntVector WorldToGrid(FVector WorldPos) const;
    FVector GridToWorld(FIntVector GridPos) const;

private:
    // --- DATI INTERNI ---

    // Cache risultati (Thread Safe solo in lettura/scrittura da GameThread)
    TMap<FIntVector, FVoxelData> VoxelCache;

    // Coda di lavoro per la fisica
    TArray<FIntVector> GenerationQueue;

    // Mappa per associare le richieste Async (Handle) alle coordinate
    TMap<FTraceHandle, FIntVector> PendingTraceHandles;

    // Delegate per la callback della fisica
    FOverlapDelegate OverlapDelegate;

    // --- LOGICA ---

    void ProcessQueue();
    void OnOverlapCompleted(const FTraceHandle& Handle, FOverlapDatum& Data);
};