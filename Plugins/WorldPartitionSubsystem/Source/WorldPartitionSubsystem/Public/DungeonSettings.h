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
	UPROPERTY(EditAnywhere, Config, Category = "Level Management")
	TMap<FString, FLevelInfo> LevelsTemplatesMap;

	/*True if you want to debug with a fixed seed*/
	UPROPERTY(EditAnywhere, Config, Category = "Level Management|Debug")
	bool UseFixedSeed;

	/*If UseFixedSeed is true, this seed will overwrite any seed at the start of each run*/
	UPROPERTY(EditAnywhere, Config, Category = "Level Management|Debug")
	int32 FixedSeedToUse;
};
