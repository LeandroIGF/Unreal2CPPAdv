// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "HAL/Runnable.h"
/**
 * 
 */
class UNREAL2CPP_API FMyRunnable : public FRunnable
{
public:
	FMyRunnable();
	virtual ~FMyRunnable();

	//Override FRunnable interface
	virtual bool Init() override;
	virtual uint32 Run() override;
	virtual void Exit() override;
	virtual void Stop() override;
	//End of FRunnable interface

	void ShutdownThread();

private:
	FRunnableThread* Thread;
	int32 Counter = 0;
	bool bShouldRun = true;
	
};
