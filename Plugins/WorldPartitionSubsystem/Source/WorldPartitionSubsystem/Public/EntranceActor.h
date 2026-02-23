// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Components/BoxComponent.h"
#include "EntranceActor.generated.h"

UCLASS()
class WORLDPARTITIONSUBSYSTEM_API AEntranceActor : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AEntranceActor();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Entrance Info")
	TObjectPtr<USceneComponent> LevelInstanceSpawnPoint;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Entrance Info")
	TObjectPtr<UBoxComponent> TriggerBoxComponent;
};
