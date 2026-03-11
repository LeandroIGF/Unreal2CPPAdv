// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "SoLoudSFXAsset.generated.h"

/**
 * 
 */
UCLASS()
class SOLOUDPLUGIN_API USoLoudSFXAsset : public UObject
{
	GENERATED_BODY()
public:

	UPROPERTY(VisibleAnywhere, Category = "Custom asset")
	FName AssetID;

};
