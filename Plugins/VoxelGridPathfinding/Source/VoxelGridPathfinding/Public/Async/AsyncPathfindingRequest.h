// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintAsyncActionBase.h"
#include "AsyncPathfindingRequest.generated.h"

/**
 * 
 */

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FDelayTickOutputPin, int32, SecondsRemaining);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FDelayOutputPin);

UCLASS()
class VOXELGRIDPATHFINDING_API UAsyncPathfindingRequest : public UBlueprintAsyncActionBase
{
	GENERATED_BODY()
	
public:
    // Pin che sparerà ogni secondo
    UPROPERTY(BlueprintAssignable)
    FDelayTickOutputPin OnTick;

    // Pin che sparerà alla fine
    UPROPERTY(BlueprintAssignable)
    FDelayOutputPin OnCompleted;

    // Factory Method
    UFUNCTION(BlueprintCallable, meta = (BlueprintInternalUseOnly = "true", WorldContext = "WorldContextObject"), Category = "AsyncNodes")
    static UAsyncPathfindingRequest* AsyncDelayWithTick(UObject* WorldContextObject, int32 TotalSeconds);

    virtual void Activate() override;

private:
    UObject* WorldContext;
    int32 TimeLeft;
    FTimerHandle TimerHandle;

    void UpdateTick();

};
