// Fill out your copyright notice in the Description page of Project Settings.


#include "LambdaActor.h"
#include "Algo/ForEach.h"

// Sets default values
ALambdaActor::ALambdaActor()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

}

// Called when the game starts or when spawned
void ALambdaActor::BeginPlay()
{
	Super::BeginPlay();
	
	//	1. Lambda single delegate example

	OnTaskCompleteDelegate.BindLambda(
		[this](bool bSuccess)
		{
			if (bSuccess)
			{
				UE_LOG(LogTemp, Warning, TEXT("Task completed successfully!"));
			}
			else
			{
				UE_LOG(LogTemp, Warning, TEXT("Task failed."));
			}
		}
	);

	// da usare bind weak lambda quando si vuole evitare reference cycle, puntatori a nullo perche' attore distrutto
	//	2. Weak Lambda delegate example
	// esempio: timer o altri sottosistemi asincroni che potrebbero richiamare il delegate dopo che l'attore e' stato distrutto
	OnTaskCompleteWeakDelegate.BindWeakLambda
		(
		this,
		[this](bool bSuccess)
		{
			if (bSuccess)
			{
				UE_LOG(LogTemp, Warning, TEXT("Weak Lambda: Task completed successfully!"));
			}
			else
			{
				UE_LOG(LogTemp, Warning, TEXT("Weak Lambda: Task failed."));
			}
		});
	
	// multicast delegate example

	OnActionCompletedMulticastDelegate.AddLambda(
		[this](int32 Points, FString String)
		{			
			UE_LOG(LogTemp, Warning, TEXT("Multicast: Task completed successfully!"));			
		}
	);

	// OnActionCompletedMulticastDelegate.AddWeakLambda weak lambda version


	// Timer with lambda to simulate task execution after delay

	if (GetWorld())
	{
		GetWorld()->GetTimerManager().SetTimer
		(
			TimerHandle_LambdaExample,
			/*begin lambda*/
			[this]() 
			{ 
				ExecuteTask();
				//

				///
				///
				///
				///
				/// 
				/// 
			},
			/* end lambda */
			2.0f, // 2 seconds delay
			false // single shot
		);
	}
}

void ALambdaActor::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	// Clear the timer to avoid dangling references
	if (GetWorld())
	{
		GetWorld()->GetTimerManager().ClearAllTimersForObject(this);
	}

	Super::EndPlay(EndPlayReason);
	
}

// Called every frame
void ALambdaActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void ALambdaActor::ExecuteTask()
{
	// Simulate task completion
	bool bTaskSuccess = true; // or false based on some logic

	// Call the delegate to notify about task completion
	if (OnTaskCompleteDelegate.IsBound())
	{
		OnTaskCompleteDelegate.Execute(bTaskSuccess);
	}

	if (OnTaskCompleteWeakDelegate.IsBound())
	{
		OnTaskCompleteWeakDelegate.Execute(bTaskSuccess);
	}

	OnActionCompletedMulticastDelegate.Broadcast(42, TEXT("La risposta a tutto"));

}

void ALambdaActor::ExampleValue()
{
	int32 Points = 100;
	FString PlayerName = TEXT("PlayerOne");

	OnExampleValue.AddLambda(
		[=](int32 Points, FString String)
		{
			UE_LOG(LogTemp, Warning, TEXT("Player: %s, Points: %d"), *PlayerName, Points);
		}
	);

}

void ALambdaActor::ExampleRef()
{
	TArray<int32> Numbers = { 1, 7, 3, 4, 10 };
	int32 Sum = 0;

	// delegate that captures by reference
	OnExampleRef.AddLambda(
		[&](int32 Points, FString String)
		{
			for (int32 Num : Numbers)
			{
				Sum += Num;
			}
			UE_LOG(LogTemp, Warning, TEXT("Sum of Numbers: %d"), Sum);
		}
	);

	Algo::ForEach(Numbers, 
		[&Sum](int32 Value) 
		{ 
			Sum += Value;
		});
}

void ALambdaActor::SortIntegers()
{
	TArray<int32> IntArray = { 5, 2, 8, 1, 4 };


	IntArray.Sort(
		[](int32 A, int32 B)->bool
		{
			return A < B; // Ascending order
		}
	);

	// print sorted array
	for (int32 Num : IntArray)
	{
		UE_LOG(LogTemp, Warning, TEXT("Sorted Number: %d"), Num);
	}


}

void ALambdaActor::SortEnemiesByDistance(TArray<AActor*>& Enemies)
{
	FVector PlayerLocation = FVector::ZeroVector; // Example player location

	Enemies.Sort(
		[PlayerLocation](const AActor& A, const AActor& B)->bool
		{
			float DistA = FVector::DistSquared(A.GetActorLocation(), PlayerLocation);
			float DistB = FVector::DistSquared(B.GetActorLocation(), PlayerLocation);

			return DistA < DistB; // Ascending order based on distance
		}
	);
}

void ALambdaActor::EsempioRiferimento_CRASH()
{
	int32 ValoreLocale = 10;

	// ATTENZIONE: Questo causerà un crash o comportamenti indefiniti!
	// Stiamo passando l'indirizzo di 'ValoreLocale' a un timer.
	FTimerHandle Handle;
	GetWorld()->GetTimerManager().SetTimer(
		Handle,
		/*begin lambda*/
		[&ValoreLocale]()
		{
			// Quando il timer scatta (tra 1 sec), la funzione EsempioRiferimento_CRASH è già finita.
			// 'ValoreLocale' è stata rimossa dallo stack.
			// Stiamo cercando di leggere memoria spazzatura.
			UE_LOG(LogTemp, Error, TEXT("Valore: %d"), ValoreLocale);
		},
		/*end lambda*/
		1.0f, 
		false);
}

void ALambdaActor::SpawnEnemyWithID(int32 EnemyID)
{
	FVector SpawnLocation = GetActorLocation();

	// Vogliamo 'this' per chiamare SpawnActor (funzione membro)
	// Vogliamo 'EnemyID' e 'SpawnLocation' come dati fissati al momento della chiamata (copia)

	FTimerHandle TimerHandle;
	GetWorld()->GetTimerManager().SetTimer(TimerHandle,
		FTimerDelegate::CreateWeakLambda(this, [this, EnemyID, SpawnLocation]()
			{
				// Usiamo 'this' -> GetWorld()
				if (UWorld* World = GetWorld())
				{
					// Usiamo le copie locali
					UE_LOG(LogTemp, Log, TEXT("Spawno nemico %d a %s"), EnemyID, *SpawnLocation.ToString());
					// World->SpawnActor<...>(..., SpawnLocation, ...);
				}
			}),
		2.0f, false);
}


