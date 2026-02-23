// Fill out your copyright notice in the Description page of Project Settings.


#include "LevelManagementSubsystem.h"
#include "DungeonSettings.h"
#include "Engine/LevelStreamingDynamic.h"
#include "Kismet/KismetMathLibrary.h"

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

		// TODO: Should i set the seed here in initialize ? Find a better setting flow where the seed should be better set
		if (Settings->UseFixedSeed)
		{
			//DungeonRandomSeed = Settings->FixedSeedToUse;
			//UKismetMathLibrary::SetRandomStreamSeed(DungeonRandomSeed, Settings->FixedSeedToUse);
			SetRunSeed(Settings->FixedSeedToUse);
		}
		else
		{
			SetRandomRunSeed();
		}
	}
}

void ULevelManagementSubsystem::Deinitialize()
{
	Super::Deinitialize();
}

ULevelStreamingDynamic* ULevelManagementSubsystem::LoadNextRoom(FTransform LevelSpawnTransform, bool &OutSuccess, const FString& OptionalLevelNameOverride, TSubclassOf<ULevelStreamingDynamic> OptionalLevelStreamingClass, bool bLoadAsTempPackage)
{

	if(LevelsTemplatesMap.Num() == 0)
	{
		UE_LOG(LogTemp, Warning, TEXT("LevelsTemplatesMap is empty. Please check your DungeonSettings."));
		OutSuccess = false;
		return nullptr;
	}

	//Get keys from map
	TArray<FString> Keys;
	LevelsTemplatesMap.GetKeys(Keys);



	//Get random value
	int32 Indexes = Keys.Num() - 1;
	//int32 RandomValue = FMath::RandRange(0, Indexes);

	int32 RandomValue = UKismetMathLibrary::RandomIntegerFromStream(Indexes, DungeonRandomSeed);

	FString Value = Keys[RandomValue];

	//Keys.Contains(Value);

	FLevelInfo LevelToLoad = LevelsTemplatesMap.FindRef(Value);

	return ULevelStreamingDynamic::LoadLevelInstanceBySoftObjectPtr(this, LevelToLoad.LevelToLoad, LevelSpawnTransform, OutSuccess, OptionalLevelNameOverride, OptionalLevelStreamingClass, bLoadAsTempPackage);
	
}

void ULevelManagementSubsystem::SetRandomRunSeed()
{
	UKismetMathLibrary::SeedRandomStream(DungeonRandomSeed);
}

void ULevelManagementSubsystem::SetRunSeed(int32 NewSeed)
{
	UKismetMathLibrary::SetRandomStreamSeed(DungeonRandomSeed, NewSeed);
}
