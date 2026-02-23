// Fill out your copyright notice in the Description page of Project Settings.


#include "EntranceActor.h"

// Sets default values
AEntranceActor::AEntranceActor()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	TriggerBoxComponent = CreateDefaultSubobject<UBoxComponent>(TEXT("TriggerBoxComponent"));
	SetRootComponent(TriggerBoxComponent);

	LevelInstanceSpawnPoint = CreateDefaultSubobject<USceneComponent>(TEXT("LevelInstanceSpawnPoint"));
	LevelInstanceSpawnPoint->SetupAttachment(GetRootComponent());

}

// Called when the game starts or when spawned
void AEntranceActor::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AEntranceActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

