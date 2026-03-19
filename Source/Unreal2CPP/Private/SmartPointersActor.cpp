// Fill out your copyright notice in the Description page of Project Settings.


#include "SmartPointersActor.h"

// Sets default values
ASmartPointersActor::ASmartPointersActor()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void ASmartPointersActor::BeginPlay()
{
	Super::BeginPlay();

	//tshared test
	// creating a scope to demonstrate tshared pointers

	TWeakPtr<CppClassPointers> ExampleWeak;

	{
		TSharedPtr<CppClassPointers> SharedPointerExample2 = MakeShared<CppClassPointers>(TEXT("First Shared Pointer"));
		UE_LOG(LogTemp, Display, TEXT("Creating a new sharedptr"));
		UE_LOG(LogTemp, Display, TEXT("Shared count: %d"), SharedPointerExample2.GetSharedReferenceCount());
		ExampleWeak = SharedPointerExample2;

		if (ExampleWeak.IsValid())
		{
			UE_LOG(LogTemp, Display, TEXT("Weak ptr valid"));
		}
		else
		{
			UE_LOG(LogTemp, Display, TEXT("Weak ptr NOT valid"));
		}

		{
			UE_LOG(LogTemp, Display, TEXT("Shared count: %d"), SharedPointerExample2.GetSharedReferenceCount());
			TSharedPtr<CppClassPointers> SecondSharedPtr = SharedPointerExample2;
			UE_LOG(LogTemp, Display, TEXT("Shared count: %d"), SharedPointerExample2.GetSharedReferenceCount());
			
		}
		UE_LOG(LogTemp, Display, TEXT("Shared count: %d"), SharedPointerExample2.GetSharedReferenceCount());

	}

	if (ExampleWeak.IsValid())
	{
		UE_LOG(LogTemp, Display, TEXT("Weak ptr valid"));
	}
	else
	{
		UE_LOG(LogTemp, Display, TEXT("Weak ptr NOT valid"));
	}


	// unique ptr

	TUniquePtr<CppClassPointers> UniqueExample = MakeUnique<CppClassPointers>(TEXT("First Unique Pointer"));
	// TUniquePtr<CppClassPointers> UniqueExample2 = UniqueExample; //Error
	{
		UE_LOG(LogTemp, Display, TEXT("UNIQUE VALID BEFORE MOVE: %i"), UniqueExample.IsValid());
		TUniquePtr<CppClassPointers> UniqueExample3 = MoveTemp(UniqueExample);
		UE_LOG(LogTemp, Display, TEXT("UNIQUE VALID AFTER MOVE: %i"), UniqueExample.IsValid());
		UE_LOG(LogTemp, Display, TEXT("UNIQUE VALID MOVED : %i"), UniqueExample3.IsValid());

	}

	UE_LOG(LogTemp, Display, TEXT("UNIQUE VALID AFTER scope: %i"), UniqueExample.IsValid());
	

}

// Called every frame
void ASmartPointersActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

