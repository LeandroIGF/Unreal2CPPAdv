// Fill out your copyright notice in the Description page of Project Settings.


#include "LevelManagementSubsystem.h"
#include "DungeonSettings.h"
#include "Engine/LevelStreamingDynamic.h"

ULevelManagementSubsystem::ULevelManagementSubsystem()
{
}

void ULevelManagementSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);
	// Load Settings
	const UDungeonSettings* Settings = GetDefault<UDungeonSettings>();
	if (Settings)
	{
		// Load Levels Templates Map from Settings
		LevelsTemplatesMap = Settings->LevelsTemplatesMap;

		if (Settings->UseFixedSeed)
		{
			//DungeonRandomSeed = Settings->FixedSeedToUse.;
		}
		else
		{

		}
	}
}

void ULevelManagementSubsystem::Deinitialize()
{
	Super::Deinitialize();
}

ULevelStreamingDynamic* ULevelManagementSubsystem::LoadNextRoom(FTransform LevelSpawnTransform, bool &OutSuccess, const FString& OptionalLevelNameOverride, TSubclassOf<ULevelStreamingDynamic> OptionalLevelStreamingClass, bool bLoadAsTempPackage)
{
	//Get keys from map
	TArray<FString> Keys;
	LevelsTemplatesMap.GetKeys(Keys);

	//Get random value
	int32 Indexes = Keys.Num() - 1;
	int32 RandomValue = FMath::RandRange(0, Indexes);

	FString Value = Keys[RandomValue];

	//Keys.Contains(Value);

	FLevelInfo LevelToLoad = LevelsTemplatesMap.FindRef(Value);

	return ULevelStreamingDynamic::LoadLevelInstanceBySoftObjectPtr(this, LevelToLoad.LevelToLoad, LevelSpawnTransform, OutSuccess, OptionalLevelNameOverride, OptionalLevelStreamingClass, bLoadAsTempPackage);
	
}
