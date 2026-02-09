// Fill out your copyright notice in the Description page of Project Settings.


#include "Async/AsyncPathfindingRequest.h"
#include "TimerManager.h"
#include "Engine/World.h"

UAsyncPathfindingRequest* UAsyncPathfindingRequest::AsyncDelayWithTick(UObject* WorldContextObject, int32 TotalSeconds)
{
    UAsyncPathfindingRequest* Node = NewObject<UAsyncPathfindingRequest>();
    Node->WorldContext = WorldContextObject;
    Node->TimeLeft = TotalSeconds;
    return Node;
}

void UAsyncPathfindingRequest::Activate()
{
    if (!WorldContext) return;

    // Avviamo un timer che chiama UpdateTick ogni secondo (looping = true)
    WorldContext->GetWorld()->GetTimerManager().SetTimer(
        TimerHandle,
        this,
        &UAsyncPathfindingRequest::UpdateTick,
        1.0f,
        true
    );

    // Eseguiamo il primo tick immediatamente per mostrare il valore iniziale
    OnTick.Broadcast(TimeLeft);
}

void UAsyncPathfindingRequest::UpdateTick()
{
    TimeLeft--;

    if (TimeLeft > 0)
    {
        // Notifichiamo il Blueprint del tempo rimanente
        OnTick.Broadcast(TimeLeft);
    }
    else
    {
        // Fermiamo il timer
        WorldContext->GetWorld()->GetTimerManager().ClearTimer(TimerHandle);

        // Notifichiamo la fine e distruggiamo il nodo
        OnCompleted.Broadcast();
        SetReadyToDestroy();
    }
}
