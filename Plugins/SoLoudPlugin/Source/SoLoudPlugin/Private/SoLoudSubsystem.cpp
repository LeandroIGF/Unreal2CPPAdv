// Fill out your copyright notice in the Description page of Project Settings.


#include "SoLoudSubsystem.h"

#include "Misc/Paths.h"

void USoLoudSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
    Super::Initialize(Collection);

    // 1. Creiamo l'istanza con MakeUnique (nessun 'new' esplicito!)
    SoloudEngine = MakeUnique<SoLoud::Soloud>();

    // 2. Inizializziamo il motore (attiva il thread audio di WASAPI)
    SoloudEngine->init();

    UE_LOG(LogTemp, Warning, TEXT("SoLoud Subsystem Inizializzato!"));
}

void USoLoudSubsystem::Deinitialize()
{
    if (SoloudEngine.IsValid())
    {
        // 1. Ferma tutti i suoni
        SoloudEngine->stopAll();

        // 2. Chiudi il backend audio (fondamentale per rilasciare WASAPI)
        SoloudEngine->deinit();

        // 3. Ora lo smart pointer può morire in pace
        //SoloudEngine.Reset();
    }
    Super::Deinitialize();
}

void USoLoudSubsystem::PlayTestSound(FString FilePath)
{
    if (SoloudEngine.IsValid())
    {
        // Carichiamo un file .wav (percorso assoluto per ora)
        TestSound.load(TCHAR_TO_ANSI(*FilePath));

        // Suoniamo!
        SoloudEngine->play(TestSound);
    }
}

void USoLoudSubsystem::UpdateSoLoudListener(FVector Location, FVector Forward, FVector Up)
{
    if (SoloudEngine.IsValid())
    {
        // Unreal usa i centimetri, SoLoud preferisce i metri. Dividiamo per 100.
        SoloudEngine->set3dListenerParameters(
            Location.X / 100.0f, Location.Y / 100.0f, Location.Z / 100.0f, // Posizione
            Forward.X, Forward.Y, Forward.Z,                             // Direzione Sguardo
            Up.X, Up.Y, Up.Z                                             // Direzione Alto
        );

        // Applica i cambiamenti a tutti i suoni attivi
        SoloudEngine->update3dAudio();
    }
}

int32 USoLoudSubsystem::Play3DSound(FString FullPath, FVector Location)
{
    if (SoloudEngine.IsValid())
    {
        // Carichiamo il suono (meglio se fatto una volta sola in una TMap)
        Sound3d.load(TCHAR_TO_ANSI(*FullPath));

        // Impostiamo il suono come 3D
        Sound3d.set3dMinMaxDistance(1.0f, 50.0f); // Udibile da 1m a 50m
        Sound3d.set3dAttenuation(SoLoud::AudioSource::INVERSE_DISTANCE, 1.0f);

        // Play3d restituisce un handle (ID) unico per quel suono specifico
        unsigned int Handle = SoloudEngine->play3d(
            Sound3d,
            Location.X / 100.0f, Location.Y / 100.0f, Location.Z / 100.0f
        );

        return (int32)Handle;
    }
    return 0;
}
