// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "SmartPointersActor.generated.h"

class CppClassPointers
{
public:

	CppClassPointers(const FString& Debug) : DebugPhrase(Debug)
	{
		UE_LOG(LogTemp, Display, TEXT("Constructor: %s"), *Debug);
	}

	virtual ~CppClassPointers() { UE_LOG(LogTemp, Display, TEXT("~Destructor: %s"), *DebugPhrase); }

	FString DebugPhrase = "";
};




UCLASS()
class UNREAL2CPP_API ASmartPointersActor : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ASmartPointersActor();

	// pointers in unreal

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "SmartPointers")
	TObjectPtr<UStaticMeshComponent> StaticMesh;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "SmartPointers")
	AActor* BrotherActor;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "SmartPointers")
	TWeakObjectPtr<AActor> ActorTarget;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "SmartPointers")
	TSubclassOf<AActor> ActorToSpawn;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "SmartPointers")
	TSoftObjectPtr<UStaticMesh> AssetPATH;

	// SMART POINTERS C++

	TSharedPtr<CppClassPointers> SharedPointerExample;	//std::shared_ptr<>, std::make_shared<>()
	TUniquePtr<CppClassPointers> UniquePointerExample;	//std::unique_ptr<>, std::make_unique<>()
	TWeakPtr<CppClassPointers> WeakPointerExample;		//std::weak_ptr<>


protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

};
