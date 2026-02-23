// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DeveloperSettings.h"
#include "LevelManagementSubsystem.h"
#include "DungeonSettings.generated.h"



/**
 * 
 */
UCLASS(Config = Game, defaultconfig, meta = (DisplayName = "Dungeon Settings"))
class WORLDPARTITIONSUBSYSTEM_API UDungeonSettings : public UDeveloperSettings
{
	GENERATED_BODY()
	
public:

	UDungeonSettings();

	/*Table of levels to load from*/
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Level Management")
	TMap<FString, FLevelInfo> LevelsTemplatesMap;


	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Level Management|Debug")
	bool UseFixedSeed;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Level Management|Debug")
	int32 FixedSeedToUse;
};
