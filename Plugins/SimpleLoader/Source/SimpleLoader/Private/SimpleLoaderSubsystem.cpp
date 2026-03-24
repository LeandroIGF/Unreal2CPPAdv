// Fill out your copyright notice in the Description page of Project Settings.

#include "SimpleLoaderSubsystem.h"

void USimpleLoaderSubsystem::Initialize(FSubsystemCollectionBase &Collection)
{
    Super::Initialize(Collection);
}
void USimpleLoaderSubsystem::Deinitialize()
{
    ActiveHandlesMap.Empty();
    Super::Deinitialize();
}

void USimpleLoaderSubsystem::SimpleStartAsyncLoading(TArray<FPrimaryAssetId> AssetsToLoad, TArray<FName> Bundles) 
{
      UAssetManager &Manager = UAssetManager::Get();      

      // Start loading the assets asynchronously and store the handle to track
      // progress
      LoadingHandle = Manager.LoadPrimaryAssets(
          AssetsToLoad, Bundles,
          FStreamableDelegate::CreateUObject(
              this, &USimpleLoaderSubsystem::OnLoadCompleted));

      if (LoadingHandle.IsValid()) 
      {          
        // Start a timer to periodically check the loading progress
        GetWorld()->GetTimerManager().SetTimer(
            TimerHandle, this, &USimpleLoaderSubsystem::UpdateProgress, 0.1f, true);
      }
}

void USimpleLoaderSubsystem::LoadPrimaryAssetsWithBundles(TArray<FPrimaryAssetId> AssetsToLoad, TArray<FName> Bundles)
{
    UAssetManager& AssetManager = UAssetManager::Get();

    LoadingHandle = AssetManager.LoadPrimaryAssets(
        AssetsToLoad, Bundles,
        FStreamableDelegate::CreateUObject(this, &USimpleLoaderSubsystem::OnLoadCompleted)
    );

    if (LoadingHandle.IsValid())
    {
        if (!IsValid(GetWorld()))
        {
            UE_LOG(LogTemp, Display, TEXT("WORLD NOT VALID!"));
            return;
        }

        GetWorld()->GetTimerManager().SetTimer(
            TimerHandle, this, &USimpleLoaderSubsystem::UpdateProgress, 0.1, true);
    }
}

float USimpleLoaderSubsystem::GetLoadingProgress() const 
{
    return LoadingHandle.IsValid() ? LoadingHandle->GetProgress() : 0.f;
}

void USimpleLoaderSubsystem::UpdateProgress() 
{
      if (LoadingHandle.IsValid()) 
      {
            float CurrentProgress = LoadingHandle->GetProgress();
            OnProgressUpdated.Broadcast(CurrentProgress);

                // If loading is complete, clear the timer to stop checking progress
                if (LoadingHandle->HasLoadCompleted()) 
                {
                    GetWorld()->GetTimerManager().ClearTimer(TimerHandle);
                }
      }
}

void USimpleLoaderSubsystem::OnLoadCompleted() 
{
      OnLoadingFinished.Broadcast();
      LoadingHandle.Reset(); // Reset the handle to indicate loading is complete
}

void USimpleLoaderSubsystem::LoadAndRelease(FPrimaryAssetId AssetId, FOnSingleAssetLoaded OnComplete) 
{
      UAssetManager &Manager = UAssetManager::Get();

      // Creiamo un handle locale temporaneo
      TSharedPtr<FStreamableHandle> TempHandle = Manager.LoadPrimaryAsset(
          AssetId, TArray<FName>(),
          FStreamableDelegate::CreateUObject(
              this, &USimpleLoaderSubsystem::OnSingleLoadFinished, AssetId,
              OnComplete));

      // Lo salviamo in una mappa per evitare che venga pulito PRIMA che finisca il caricamento
      ActiveHandlesMap.Add(AssetId, TempHandle);
}

void USimpleLoaderSubsystem::OnSingleLoadFinished(FPrimaryAssetId AssetId, FOnSingleAssetLoaded OnComplete) 
{
      // 1. Recuperiamo l'handle dalla mappa
      TSharedPtr<FStreamableHandle> Handle = ActiveHandlesMap.FindRef(AssetId);

      if (Handle.IsValid()) 
      {
            UObject *Asset = Handle->GetLoadedAsset();

            // 2. "Consegniamo" l'asset al chiamante (es. il Blueprint)
            // Il Blueprint ricever� l'asset e DOVR� salvarlo in una variabile UPROPERTY
            OnComplete.ExecuteIfBound(Asset);

            // 3. PULIZIA: Rimuoviamo l'handle dalla mappa.
            // L'handle viene distrutto, ma l'asset � salvo se il chiamante lo ha salvato!
            ActiveHandlesMap.Remove(AssetId);
            UE_LOG(LogTemp, Log,
                   TEXT("Handle rimosso per l'asset %s. Memoria ora gestita dal ricevente."),
                   *AssetId.ToString());
      }
}

UPrimaryDataAsset* USimpleLoaderSubsystem::GetLoadedPrimaryAsset(FPrimaryAssetId AssetId, TSubclassOf<UPrimaryDataAsset> AssetClass)
{
    // Get Manager
    UObject* ObjectLoaded = UAssetManager::Get().GetPrimaryAssetObject(AssetId);

    //UAssetManager::Get().GetPrimaryAssetIdList(FPrimaryAssetType("Orc"), AllOrcs);

    UPrimaryDataAsset* LoadedPrimaryAsset = Cast<UPrimaryDataAsset>(ObjectLoaded);
    // Check if the same type
    if (LoadedPrimaryAsset && LoadedPrimaryAsset->IsA(AssetClass))
    {
        return LoadedPrimaryAsset;
    }
    return nullptr;
}

/* Version of on load to get a specific asset

void UMyLoadingSubsystem::OnLoadCompleted()
{
    if (LoadingHandle.IsValid())
    {
        TArray<UObject*> LoadedAssets;
        LoadingHandle->GetLoadedAssets(LoadedAssets);

        for (UObject* Asset : LoadedAssets)
        {
            // Do casting (es. a UMyWeaponAsset) e usarli
            UE_LOG(LogTemp, Log, TEXT("Asset caricato: %s"), *Asset->GetName());
        }
    }
}

//If handle still alive, this function will give the get primary asset object
UMyWeaponAsset* Weapon = Manager.GetPrimaryAssetObject<UMyWeaponAsset>(WeaponId);

AssetManager.ChangeBundleState(AssetId, BundlesToKeep, BundlesToRemove);


*/