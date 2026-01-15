// Fill out your copyright notice in the Description page of Project Settings.


#include "EnemiesConfig.h"
#include "HAL/IConsoleManager.h"

// Console variable declaration, remember to include "HAL/IConsoleManager.h"
// <variable type>
static TAutoConsoleVariable<float> CVarEnemyGlobalHealthMult //CVar"CVarName"
(
	TEXT("Enemy.GlobalHealthMult"),							// Name to show inside the console variable
	1.0f,													// Default value
	TEXT("Health Multiplier. Saved in EnemySettings.ini"),	// Tooltip for the console variable
	ECVF_Default											// Flag of the CVar
);

// Cvar flags

/*
	ECVF_Default			Nessun comportamento speciale.
	ECVF_Cheat				Importante: La variabile esiste nelle build di sviluppo, ma viene nascosta o rimossa nelle build Shipping. Usalo per i trucchi di debug.
	ECVF_Scalability		Indica che questa variabile cambia la qualità grafica (utile per i menu delle opzioni grafiche).
	ECVF_RenderThreadSafe	Indica che è sicuro leggere questa variabile anche dal Render Thread (utile per shader o parametri grafici).
	ECVF_ReadOnly			La variabile può essere letta ma non modificata dalla console (utile per esporre costanti di sistema per debug).
*/

// callback for when our cvar changes
static void OnHealthCVarChanged(IConsoleVariable* Var);


UEnemiesConfig* UEnemiesConfig::Get()
{
	//return GetDefault<UEnemiesConfig>();

	return GetMutableDefault<UEnemiesConfig>();
}



void OnHealthCVarChanged(IConsoleVariable* Var)
{
	float NewValue = Var->GetFloat();

	UEnemiesConfig* Settings = UEnemiesConfig::Get();

	if (Settings->GlobalHealthMultiplier != NewValue)
	{
		Settings->GlobalHealthMultiplier = NewValue;

		Settings->SaveConfig(CPF_Config, *Settings->GetDefaultConfigFilename());

		UE_LOG(LogTemp, Log, TEXT("Enemy Config Saved: new value: %f. Saved in: %s"), NewValue, *Settings->GetDefaultConfigFilename());
	}
}

// should be called only once , when the config object is created, in editor or at runtime
void UEnemiesConfig::PostInitProperties()
{
	Super::PostInitProperties();

	if (IsTemplate())
	{
		CVarEnemyGlobalHealthMult->Set(GlobalHealthMultiplier, ECVF_SetByProjectSetting);

		CVarEnemyGlobalHealthMult.AsVariable()->SetOnChangedCallback(FConsoleVariableDelegate::CreateStatic(&OnHealthCVarChanged));
	}
}