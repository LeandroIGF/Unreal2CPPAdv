// Fill out your copyright notice in the Description page of Project Settings.


#include "FMyRunnable.h"

FMyRunnable::FMyRunnable()
{
	Thread = FRunnableThread::Create(this, TEXT("FMyRunnableThread"), 0, TPri_BelowNormal);
}

FMyRunnable::~FMyRunnable()
{
	ShutdownThread();
}

bool FMyRunnable::Init()
{
	UE_LOG(LogTemp, Display, TEXT("WORKER INIT"));
	return true;
}

uint32 FMyRunnable::Run()
{
	UE_LOG(LogTemp, Display, TEXT("WORKER RUN"));

	while (bShouldRun)
	{
		Counter++;
		FPlatformProcess::Sleep(1.0f);
		UE_LOG(LogTemp, Display, TEXT("Counter: %d"), Counter);
		if (Counter>=10)
		{
			bShouldRun = false;
		}
	}
	return 0;
}

void FMyRunnable::Exit()
{
	UE_LOG(LogTemp, Display, TEXT("WORKER EXIT"));
}

void FMyRunnable::Stop()
{
	UE_LOG(LogTemp, Display, TEXT("WORKER STOP"));
	bShouldRun = false;
}

void FMyRunnable::ShutdownThread()
{
	if (Thread)
	{
		Thread->Kill(true);
		delete Thread;
		Thread = nullptr;
	}
}
