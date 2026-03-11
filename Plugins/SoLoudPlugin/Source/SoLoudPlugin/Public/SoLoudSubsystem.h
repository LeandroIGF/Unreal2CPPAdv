// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "Templates/UniquePtr.h" // Per TUniquePtr
#include "soloud.h"             // Header della libreria
#include "soloud_wav.h"         // Per i file .wav
#include "SoLoudSubsystem.generated.h"

UCLASS()
class SOLOUDPLUGIN_API USoLoudSubsystem : public UGameInstanceSubsystem
{
    GENERATED_BODY()

public:
    // Ciclo di vita del Subsystem
    virtual void Initialize(FSubsystemCollectionBase& Collection) override;
    virtual void Deinitialize() override;

    UFUNCTION(BlueprintCallable, Category = "SoLoud")
    void PlayTestSound(FString FilePath);


    UFUNCTION(BlueprintCallable, Category = "SoLoud")
    void UpdateSoLoudListener(FVector Location, FVector Forward, FVector Up);

    UFUNCTION(BlueprintCallable, Category = "SoLoud")
    int32 Play3DSound(FString FullPath, FVector Location);

private:
    // --- LO SMART POINTER ---
    // Gestiamo l'istanza del motore audio in modo sicuro
    TUniquePtr<SoLoud::Soloud> SoloudEngine;

    // Un contenitore per il suono (SoLoud gestisce i dati audio qui)
    SoLoud::Wav TestSound;

    // Un contenitore per il suono (SoLoud gestisce i dati audio qui)
    SoLoud::Wav Sound3d;
};