#include "VoxelTestMultithread.h"
#include "Async/Async.h"
//#include "VoxelActor.h"

AVoxelTestMultithread::AVoxelTestMultithread()
{
    PrimaryActorTick.bCanEverTick = true;

    // 1. Volume di delimitazione (Bounds)
    BoundsVolume = CreateDefaultSubobject<UBoxComponent>(TEXT("BoundsVolume"));
    RootComponent = BoundsVolume;
    BoundsVolume->SetBoxExtent(FVector(5000, 5000, 1000));
    BoundsVolume->SetCollisionProfileName(TEXT("NoCollision"));

    // 2. HISM per renderizzare i muri (Optimized)
    HISM_Blocked = CreateDefaultSubobject<UHierarchicalInstancedStaticMeshComponent>(TEXT("HISM_Blocked"));
    HISM_Blocked->SetupAttachment(RootComponent);
    // IMPORTANTE: Disabilitiamo collisione e ombre sul render per performance estreme
    HISM_Blocked->SetCollisionEnabled(ECollisionEnabled::NoCollision);
    HISM_Blocked->SetCastShadow(false);
    // Opzionale: Disabilita collisione navigazione se non serve
    HISM_Blocked->SetCanEverAffectNavigation(false);

    // 3. Bind del delegate per la risposta asincrona
    OverlapDelegate.BindUObject(this, &AVoxelTestMultithread::OnOverlapCompleted);
}

// Chiamata quando cambi proprietà nell'editor (per vedere Mesh e Materiale aggiornati)
void AVoxelTestMultithread::OnConstruction(const FTransform& Transform)
{
    Super::OnConstruction(Transform);

    if (ObstacleMesh)
    {
        HISM_Blocked->SetStaticMesh(ObstacleMesh);
    }
    if (ObstacleMaterial)
    {
        HISM_Blocked->SetMaterial(0, ObstacleMaterial);
    }
}

void AVoxelTestMultithread::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);

    // Se c'è lavoro in coda, processalo
    if (GenerationQueue.Num() > 0)
    {
        ProcessQueue();
    }
}

// --- FASE 1: CALCOLO MATEMATICO (BACKGROUND THREAD) ---
void AVoxelTestMultithread::RequestRegionAsync(FVector Center, float Radius)
{
    // Copiamo i valori per passarli al thread in sicurezza
    float LocalVoxelSize = VoxelSize;

    Async(EAsyncExecution::ThreadPool, [this, Center, Radius, LocalVoxelSize]()
        {
            // SIAMO IN BACKGROUND
            TArray<FIntVector> MathCoords;
            int32 GridRadius = FMath::CeilToInt(Radius / LocalVoxelSize);

            // Calcoliamo il diametro (lato del cubo di ricerca)
            int32 SideLength = GridRadius * 2;
            // Riserviamo memoria facendo lato * lato * lato (Volume)
            MathCoords.Reserve(SideLength * SideLength * SideLength);

            FIntVector CenterGrid = WorldToGrid(Center);

            // Loop intensivo matematico
            for (int32 x = -GridRadius; x <= GridRadius; x++)
            {
                for (int32 y = -GridRadius; y <= GridRadius; y++)
                {
                    for (int32 z = -GridRadius; z <= GridRadius; z++)
                    {
                        // Check sferico semplice
                        if ((x * x + y * y + z * z) <= (GridRadius * GridRadius))
                        {
                            MathCoords.Add(CenterGrid + FIntVector(x, y, z));
                        }
                    }
                }
            }

            // TORNIAMO AL GAME THREAD
            AsyncTask(ENamedThreads::GameThread, [this, MathCoords]()
                {
                    for (const FIntVector& Coord : MathCoords)
                    {
                        // Aggiungi alla coda solo se non è già stato calcolato/renderizzato
                        if (!VoxelCache.Contains(Coord))
                        {
                            // Nota: AddUnique su grandi array è lento, ma per ora va bene.
                            // Se noti rallentamenti qui, usa un TSet di appoggio.
                            GenerationQueue.AddUnique(Coord);
                        }
                    }
                });
        });
}

// --- FASE 2: DISPATCHER FISICA (GAME THREAD TIME-SLICED) ---
void AVoxelTestMultithread::ProcessQueue()
{
    int32 SentCount = 0;

    while (GenerationQueue.Num() > 0 && SentCount < MaxPhysicsRequestsPerFrame)
    {
        FIntVector CurrentCoord = GenerationQueue.Pop();

        // Controllo se nel frattempo è stato calcolato
        if (VoxelCache.Contains(CurrentCoord)) continue;

        FVector WorldPos = GridToWorld(CurrentCoord);

        // Bounds Check (Siamo dentro il volume di sicurezza?)
        FVector LocalPos = GetActorTransform().InverseTransformPosition(WorldPos);
        FBox Bounds = FBox(-BoundsVolume->GetScaledBoxExtent(), BoundsVolume->GetScaledBoxExtent());

        if (!Bounds.IsInside(LocalPos)) continue;

        // Setup Async Physics
        FCollisionQueryParams Params;
        Params.bTraceComplex = false;
        // Box leggermente più piccolo per evitare false collisioni adiacenti
        FCollisionShape BoxShape = FCollisionShape::MakeBox(FVector(VoxelSize / 2.0f) * 0.95f);

        // Lancio richiesta (Non bloccante)
        FTraceHandle Handle = GetWorld()->AsyncOverlapByChannel(
            WorldPos,
            FQuat::Identity,
            ECC_WorldStatic, // Canale: Controlla WorldStatic (Muri, ecc.)
            BoxShape,
            Params,
            FCollisionResponseParams::DefaultResponseParam,
            &OverlapDelegate
        );

        PendingTraceHandles.Add(Handle, CurrentCoord);
        SentCount++;
    }
}



// --- FASE 3: RISULTATO E RENDERING (CALLBACK) ---
void AVoxelTestMultithread::OnOverlapCompleted(const FTraceHandle& Handle, FOverlapDatum& Data)
{
    if (FIntVector* FoundCoord = PendingTraceHandles.Find(Handle))
    {
        FIntVector Coord = *FoundCoord;
        PendingTraceHandles.Remove(Handle);

        // Se OutOverlaps ha elementi, abbiamo colpito qualcosa -> BLOCKED
        bool bHit = Data.OutOverlaps.Num() > 0;

        // Salvataggio dati
        FVoxelData NewData;
        NewData.State = bHit ? EVoxelState::Blocked : EVoxelState::Free;
        VoxelCache.Add(Coord, NewData);

        // RENDERING: Disegniamo SOLO se è un ostacolo (bHit == true)
        if (bHit)
        {
            FVector WorldPos = GridToWorld(Coord);

            // Calcolo Scala: Il cubo default UE5 è 100x100x100.
            // Se VoxelSize è 50, scale deve essere 0.5.
            float Scale = VoxelSize / 100.0f;

            FTransform InstanceTransform;
            InstanceTransform.SetLocation(WorldPos);
            InstanceTransform.SetScale3D(FVector(Scale));

            // HISM renderizza questo istantaneamente insieme agli altri
            HISM_Blocked->AddInstance(InstanceTransform);
        }
        // Se è Free (bHit == false), NON facciamo nulla visivamente.
    }
}

// --- HELPERS ---


FIntVector AVoxelTestMultithread::WorldToGrid(FVector WorldPos) const
{
    return FIntVector(
        FMath::FloorToInt(WorldPos.X / VoxelSize),
        FMath::FloorToInt(WorldPos.Y / VoxelSize),
        FMath::FloorToInt(WorldPos.Z / VoxelSize)
    );
}

FVector AVoxelTestMultithread::GridToWorld(FIntVector GridPos) const
{
    return FVector(
        (GridPos.X * VoxelSize) + (VoxelSize / 2.0f),
        (GridPos.Y * VoxelSize) + (VoxelSize / 2.0f),
        (GridPos.Z * VoxelSize) + (VoxelSize / 2.0f)
    );
}