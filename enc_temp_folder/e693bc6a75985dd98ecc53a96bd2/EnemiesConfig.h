// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DeveloperSettings.h"
#include "Engine/DataTable.h"
#include "EnemiesConfig.generated.h"

/**
 * Config: EnemiesSettings
 * Progetto ->		Config/DefaultEnemiesSettings.ini
 * Utente/Build ->	Saved/Config/Windows/EnemiesSettings.ini * 
 */

UCLASS(Config = EnemiesSettings, defaultconfig, meta = (DisplayName = "Enemies Configuration"))
class UNREAL2CPP_API UEnemiesConfig : public UDeveloperSettings
{
	GENERATED_BODY()
	
public:

	// Sections of the settings in the editor
	virtual FName GetContainerName() const override { return FName("Project"); }
	virtual FName GetCategoryName() const override { return FName("Game"); }
	virtual FName GetSectionName() const override { return FName("Enemies"); }

	// Enemy Data Table with all the info to use for spawning enemies, like meshes, animations, stats, etc.
	UPROPERTY(Config, EditAnywhere, BlueprintReadOnly, Category = "Assets", meta = (AllowedClasses = "DataTable"))
	TSoftObjectPtr<UDataTable> EnemyDataTable;

	// Global multiplier applied to all enemy health values
	UPROPERTY(Config, EditAnywhere, BlueprintReadOnly, Category = "Balance", meta = (ClampMin = "0.1"))
	float GlobalHealthMultiplier = 1.0f;

	// Debug option to show enemy info (like health, state, etc.)
	UPROPERTY(Config, EditAnywhere, BlueprintReadOnly, Category = "Debug")
	bool bShowEnemyDebug = false;

	// Helper to get the config instance
	UFUNCTION(BlueprintCallable, BlueprintPure)
	static UEnemiesConfig* Get();

	virtual void PostInitProperties() override;
};
