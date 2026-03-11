// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "SoLoudAudioAsset.generated.h"

/**
 * 
 */
UCLASS(BlueprintType)
class SOLOUDPLUGIN_API USoLoudAudioAsset : public UObject
{
	GENERATED_BODY()
	
public:
	/** Dati grezzi del file (formato originale .wav, .ogg, ecc.) */
	UPROPERTY()
	TArray<uint8> RawAudioData;

	// Se vuoi vedere info utili nell'editor senza crashare:
	UPROPERTY(VisibleAnywhere, Category = "AudioStats")
	int32 SampleRate = 44100;

	UPROPERTY(VisibleAnywhere, Category = "AudioStats")
	float Duration = 0.f;
};
