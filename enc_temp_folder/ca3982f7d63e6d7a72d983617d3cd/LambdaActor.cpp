// Fill out your copyright notice in the Description page of Project Settings.


#include "LambdaActor.h"
#include "Algo/ForEach.h"
#include "Async/Async.h"

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

void ALambdaActor::DoHeavyCalculationThread()
{

	uint32 GameThreadId = FPlatformTLS::GetCurrentThreadId();
	UE_LOG(LogTemp, Warning, TEXT("ID Game Thread: %d"), GameThreadId);

	

	Async(EAsyncExecution::ThreadPool,
		[this]()
		{

			TArray<int32> ItemsToProcess;

			uint32 GameThreadId = FPlatformTLS::GetCurrentThreadId();
			UE_LOG(LogTemp, Warning, TEXT("ID Game Thread: %d"), GameThreadId);

			ResultActor = 0;

			// 1. calcolo pesante
			int32 Result = ResultActor;
			for (int i = 0; i < Iterations; i++)
			{
				Result += i;
				//calcolo da fare...
				ItemsToProcess.Add(i);
				//UE_LOG(LogTemp, Display, TEXT("Added: %d"), i);
			}

			// 2. Torniamo nel nostro game thread
			/*
			Async(EAsyncExecution::TaskGraph,
				[this, Result, MovedArray = MoveTemp(ItemsToProcess)]()
				{
					ResultArrayActor = MovedArray;
					this->ResultActor = Result;
				}
			);
			
			*/

			AsyncTask(ENamedThreads::GameThread,
				[this, Result, MovedArray = MoveTemp(ItemsToProcess)]() mutable
				{
					//ResultArrayActor = MovedArray;
					Swap(this->ResultArrayActor, MovedArray);
					this->ResultActor = Result;

					// da fare solo se chiamato ogni tanto e non in maniera continua
					Async(EAsyncExecution::ThreadPool, [ArrayToDestroy = MoveTemp(MovedArray)]()
					{
						// L'array muore qui, nel background, senza bloccare il gioco.
					});
				}
			);
		}
	);
	
}

void ALambdaActor::DoHeavyCalculationGameThread()
{
	TArray<int32> ItemsToProcess;
	ResultActor = 0;
	int32 Result = ResultActor;
	for (int i = 0; i < Iterations; i++)
	{
		Result += i;
		//ResultArrayActor;
		ItemsToProcess.Add(i);
		//UE_LOG(LogTemp, Display, TEXT("Added: %d"), i);
	}

	ResultArrayActor = ItemsToProcess;

	UE_LOG(LogTemp, Display, TEXT("Finito di controllare tutti gli elementi"));

	ResultActor = Result;
}

//Esercizio 1
void ALambdaActor::Esercizio1_FindPrimeNumbers()
{
	//
	Async(EAsyncExecution::Thread,
		[this]()
		{
			//
			int32 Count = 0;
			int32 MaxIteration = 200000;

			for (int32 i = 2; i < MaxIteration; i++)
			{
				bool bIsPrime = true;

				for (int j = 2; j * j <= i; j++)
				{
					if (i % j == 0)
					{
						bIsPrime = false;
						break;
					}
				}
				if (bIsPrime) Count++;
			}

			AsyncTask(ENamedThreads::GameThread,
				[this, Count]()
				{
					this->PrimeNumbersResult = Count;
					UE_LOG(LogTemp, Display, TEXT("Ex1 finito trovati : %d numeri primi"), Count);
				}
				);
		}

	);

}

void ALambdaActor::Esercizio2_Inventory()
{
	// Background in thread
	Async(EAsyncExecution::Thread,
		[this]()
		{
			// setup richiesta thread
			TArray<int32> LocalArray;
			LocalArray.Reserve(1000000);

			for (int32 i = 0; i < 1000000; i++)
			{
				LocalArray.Add(FMath::RandRange(0, 999999));
			}

			LocalArray.Sort();

			// Get back to gamethread
			AsyncTask(ENamedThreads::GameThread,
				[this, SortedData = MoveTemp(LocalArray)]() mutable
				{
					Swap(this->InventoryArray, SortedData);

					for (int32 Number : this->InventoryArray)
					{
						// attenzione a fare log con tanti elementi
						UE_LOG(LogTemp, Display, TEXT("%d"), Number);
					}

					// da fare solo se chiamato ogni tanto e non in maniera continua
					// clean sorted data array
					Async(EAsyncExecution::ThreadPool, [ArrayToDestroy = MoveTemp(SortedData)]()
					{
						// L'array muore qui, nel background, senza bloccare il gioco.
					});
				}
			);
		}
	);
}


