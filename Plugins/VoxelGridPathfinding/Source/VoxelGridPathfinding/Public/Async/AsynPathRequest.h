// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintAsyncActionBase.h"
#include "AsynPathRequest.generated.h"

class UVoxelGridSubsystem;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnPathRequestFinished, const TArray<FVector>&, PathPoints, int32, RequestID);

/**
 * 
 */
UCLASS()

class VOXELGRIDPATHFINDING_API UAsynPathRequest : public UBlueprintAsyncActionBase
{
	GENERATED_BODY()

public:

	// Pin che sparerà se la richiesta di pathfinding è completata con successo
    UPROPERTY(BlueprintAssignable)
    FOnPathRequestFinished OnSuccess;

	// Pin che sparerà in caso di fallimento
    UPROPERTY(BlueprintAssignable)
    FOnPathRequestFinished OnFailed;

    virtual void Activate() override;

	// Requests an asynchronous pathfinding update for the given array of path points.
	UFUNCTION(BlueprintCallable, meta = (BlueprintInternalUseOnly = "true", WorldContext = "WorldContextObject"), Category = "VoxelGrid|Async")
	static UAsynPathRequest* RequestPathfindingUpdate(UObject* WorldContextObject, FVector StartLocationWorld, FVector EndLocationWorld);

private:

	// Infromazioni interne alla richiesta di pathfinding

	bool bInvalidRequest = false;
	FVector StartLocation;
	FVector EndLocation;
	int32 RequestIDInternal;

	UObject* WorldContext;
	UVoxelGridSubsystem* VoxelSubsystem;

	UFUNCTION()
	void OnPathReceived(const TArray<FVector>& PathPoints, int32 RequestID);

};
