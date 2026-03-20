// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/AssetManager.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "SimpleLoaderSubsystem.generated.h"



// Delegates for loading progress and completion events
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnLoadingProgressUpdated, float, Progress);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnLoadingFinished);

// Single asset for BP callback (for example, when loading a single asset with a
// specific callback)
DECLARE_DYNAMIC_DELEGATE_OneParam(FOnSingleAssetLoaded, UObject *, LoadedAsset);

/*
    // Forza l'Asset Manager a mantenere l'asset in memoria "per sempre" (fino a
   ordine contrario) UAssetManager::Get().GetPackageAsyncLoadPriority(AssetId);
    // In realt� si usa tipicamente:
    UAssetManager::Get().AddDynamicAsset(AssetId);
*/

/**
 *
 */
UCLASS()
class SIMPLELOADER_API USimpleLoaderSubsystem : public UGameInstanceSubsystem {
  GENERATED_BODY()

public:
  virtual void Initialize(FSubsystemCollectionBase& Collection) override;
  virtual void Deinitialize() override;

  // Loading of assets asynchronously using the Asset Manager
  UFUNCTION(BlueprintCallable, Category = "Loading")
  void SimpleStartAsyncLoading(TArray<FPrimaryAssetId> AssetsToLoad, TArray<FName> Bundles);


  UFUNCTION(BlueprintCallable, Category = "SimpleLoader|Load")
  void LoadPrimaryAssetsWithBundles(TArray<FPrimaryAssetId> AssetsToLoad, TArray<FName> Bundles);

  // Getting the current loading progress (0.0 to 1.0)
  UFUNCTION(BlueprintPure, Category = "Loading")
  float GetLoadingProgress() const;

  UPROPERTY(BlueprintAssignable, Category = "Loading")
  FOnLoadingProgressUpdated OnProgressUpdated;

  UPROPERTY(BlueprintAssignable, Category = "Loading")
  FOnLoadingFinished OnLoadingFinished;

  UFUNCTION(BlueprintCallable, Category = "Loading")
  void LoadAndRelease(FPrimaryAssetId AssetId, FOnSingleAssetLoaded OnComplete);

  UFUNCTION(BlueprintCallable, Category = "Loading", meta = (DeterminesOutputType = "AssetClass"))
  UPrimaryDataAsset* GetLoadedPrimaryAsset(FPrimaryAssetId AssetId, TSubclassOf<UPrimaryDataAsset> AssetClass);
  

private:
  // Handle from the Asset Manager for the current loading operation
  TSharedPtr<FStreamableHandle> LoadingHandle;

  TSharedPtr<FStreamableHandle> LoadingAssetsHandle;

  // Callback chiamata dall'Asset Manager al termine
  void OnLoadCompleted();

  // Funzione di polling per aggiornare la UI (pu essere chiamata da un Timer o
  // dal Tick)
  void UpdateProgress();

  void OnSingleLoadFinished(FPrimaryAssetId AssetId, FOnSingleAssetLoaded OnComplete);

  FTimerHandle TimerHandle;

  // Map to keep track of active loading handles
  TMap<FPrimaryAssetId, TSharedPtr<FStreamableHandle>> ActiveHandlesMap;
};
