// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "LambdaActor.generated.h"

// declare single delegate type
DECLARE_DELEGATE_OneParam(FOnTaskComplete, bool /*bSuccess*/);

// declare multicast delegate type
DECLARE_MULTICAST_DELEGATE_TwoParams(FOnActionCompleted, int32 /*ActionID*/, FString /*Result*/);


/*
Sintassi	Significato				Uso Tipico in UE5							Sicurezza Timer
[this]		Accedi a membri classe	Quasi sempre								Sicuro (se usi WeakLambda)
[=]			Copia tutto				Passare dati semplici (int, float)			Sicuro
[&]			Riferimento a tutto		Algoritmi (Sort, Filter)					Pericoloso (Crash probabile)
[Var]		Copia Var				Passare dati specifici						Sicuro
[&Var]		Riferimento a Var		Modificare var locale in loop immediato		Pericoloso per async

*/

UCLASS()
class UNREAL2CPP_API ALambdaActor : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ALambdaActor();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// 1. Example lambda with single delegate with parameter
	FOnTaskComplete OnTaskCompleteDelegate;

	FOnTaskComplete OnTaskCompleteWeakDelegate;

	FOnActionCompleted OnActionCompletedMulticastDelegate;

	FOnActionCompleted OnExampleValue;

	FOnActionCompleted OnExampleRef;

	// 2 Timer example

	FTimerHandle TimerHandle_LambdaExample;

	void ExecuteTask();

	void ExampleValue();

	void ExampleRef();

	void SortIntegers();

	void SortEnemiesByDistance(TArray<AActor*>& Enemies);

	void EsempioRiferimento_CRASH();

	void SpawnEnemyWithID(int32 EnemyID);

	// Multithread

	UFUNCTION(BlueprintCallable, Category = "Actor MultiThread")
	void DoHeavyCalculationThread();
	
	UFUNCTION(BlueprintCallable, Category = "Actor MultiThread")
	void DoHeavyCalculationGameThread();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Actor MultiThread")
	int32 ResultActor = 0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Actor MultiThread")
	int32 Iterations = 100000 ;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Actor MultiThread")
	TArray<int32> ResultArrayActor;

	// Esercizio 1
	UFUNCTION(BlueprintCallable, CallInEditor, Category = "Actor MultiThread")
	void Esercizio1_FindPrimeNumbers();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Esercizio 1")
	int32 PrimeNumbersResult;

	// Esercizio 2

	UFUNCTION(BlueprintCallable, CallInEditor, Category = "Actor MultiThread")
	void Esercizio2_Inventory();

	TArray<int32> InventoryArray;

};
