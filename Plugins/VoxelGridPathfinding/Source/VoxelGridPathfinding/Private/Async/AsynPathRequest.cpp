// Fill out your copyright notice in the Description page of Project Settings.


#include "Async/AsynPathRequest.h"
#include "VoxelGridSubsystem.h"

void UAsynPathRequest::Activate()
{
	UWorld* FoundWorld = GEngine->GetWorldFromContextObject(WorldContext, EGetWorldErrorMode::LogAndReturnNull);

	if (!IsValid(FoundWorld))
	{
		UE_LOG(LogTemp, Display, TEXT("ASYNC PATH REQUEST: NO WORLD, PROBABLY NO WORLD CONTEXT"));
		return;
	}

	VoxelSubsystem = FoundWorld->GetSubsystem<UVoxelGridSubsystem>();
	RequestIDInternal = VoxelSubsystem->GetNextIdRequest();
	
	// Bind to when the path is found
	//VoxelSubsystem->OnPathFound.AddDynamic(this, &UAsynPathRequest::OnPathFoundHandler);

	VoxelSubsystem->OnOnPathFound.AddDynamic(this, &UAsynPathRequest::OnPathReceived);
	// we make the request
	VoxelSubsystem->FindPathAsync(StartLocation, EndLocation, RequestIDInternal);		
	
	//OnSuccess.Broadcast(bSuccess, PathPoints);
}

UAsynPathRequest* UAsynPathRequest::RequestPathfindingUpdate(UObject* WorldContextObject, FVector StartLocationWorld, FVector EndLocationWorld)
{
	UAsynPathRequest* Node = NewObject<UAsynPathRequest>();
	Node->WorldContext = WorldContextObject;
	Node->RegisterWithGameInstance(WorldContextObject);
	Node->StartLocation = StartLocationWorld;
	Node->EndLocation = EndLocationWorld;
	return Node;
}

void UAsynPathRequest::OnPathReceived(const TArray<FVector>& PathPoints, int32 RequestID)
{
	//TODO: This part should be done in the subsystem, so for now we check the RequestID
	if (!(RequestIDInternal == RequestID))
	{
		UE_LOG(LogTemp, Display, TEXT("ASYNC PATH REQUEST: not the same RequestID"));
		return;
	}
	// TODO: REMOVE REQUEST ID FROM SUBSYSTEM AND CONVERT TO TMAP
	if (PathPoints.IsEmpty())
	{
		UE_LOG(LogTemp, Display, TEXT("ASYNC PATH REQUEST: PATH NOT FOUND"));
		OnFailed.Broadcast(PathPoints, RequestID);
		VoxelSubsystem->OnOnPathFound.RemoveDynamic(this, &UAsynPathRequest::OnPathReceived);
	}
	else
	{
		UE_LOG(LogTemp, Display, TEXT("ASYNC PATH REQUEST: PATH FOUND WITH %d POINTS"), PathPoints.Num());
		OnSuccess.Broadcast(PathPoints, RequestID);
		VoxelSubsystem->OnOnPathFound.RemoveDynamic(this, &UAsynPathRequest::OnPathReceived);
	}

	SetReadyToDestroy();
}




