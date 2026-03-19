// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "CPP_EnemyDataAsset.generated.h"

/**
 * 
 */
UCLASS()
class UNREAL2CPP_API UCPP_EnemyDataAsset : public UPrimaryDataAsset
{
	GENERATED_BODY()

public:

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Config")
	TSubclassOf<AActor> EnemyClass;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Config", meta = (AssetBundles = "Visuals"))
	TSoftObjectPtr<UStaticMesh> EnemyMesh;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Config", meta = (AssetBundles = "Hat"))
	TSoftObjectPtr<UStaticMesh> HatMesh;
};
