// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "Templates/UniquePtr.h"
#include "soloud.h"
#include "soloud_wav.h"
#include "SoLoudAudioAsset.h"
#include "SoLoudSubsystem.generated.h"

USTRUCT()
struct FSoLoudWavResource
{
    GENERATED_BODY()

    SoLoud::Wav* WavInstance = nullptr;

    FSoLoudWavResource() : WavInstance(nullptr) {}
};

UCLASS()
class SOLOUDPLUGIN_API USoLoudSubsystem : public UGameInstanceSubsystem
{
    GENERATED_BODY()

public:
    virtual void Initialize(FSubsystemCollectionBase& Collection) override;
    virtual void Deinitialize() override;

    UFUNCTION(BlueprintCallable, Category = "SoLoud")
    void PlayTestSound(USoLoudAudioAsset* AssetToPlay);

    UFUNCTION(BlueprintCallable, Category = "SoLoud")
    void UpdateSoLoudListener(FVector Location, FVector Forward, FVector Up);

    UFUNCTION(BlueprintCallable, Category = "SoLoud")
    int32 Play3DSound(FString FullPath, FVector Location);

    UFUNCTION(BlueprintCallable, Category = "SoLoud")
    void UnloadAudioAsset(USoLoudAudioAsset* AssetToUnload);

private:

    UPROPERTY(Transient)
    TMap<USoLoudAudioAsset*, FSoLoudWavResource> LoadedAudioResources;

    TUniquePtr<SoLoud::Soloud> SoloudEngine;

    

    // Un contenitore per il suono (SoLoud gestisce i dati audio qui)
    SoLoud::Wav TestSound;

    // Un contenitore per il suono (SoLoud gestisce i dati audio qui)
    SoLoud::Wav Sound3d;
};
