// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/CheatManager.h"
#include "Unreal2CheatManager.generated.h"

/**
 * 
 */
UCLASS()
class UNREAL2CPP_API UUnreal2CheatManager : public UCheatManager
{
	GENERATED_BODY()
	
public:
	
	UFUNCTION(Exec, BlueprintNativeEvent, Category = "Cheats")
	void GiveAllWeapons();

	UFUNCTION(Exec, BlueprintNativeEvent, Category = "Cheats")
	void GodMode();

	UFUNCTION(Exec, BlueprintNativeEvent, Category = "Cheats")
	void GodSpeed();
};
