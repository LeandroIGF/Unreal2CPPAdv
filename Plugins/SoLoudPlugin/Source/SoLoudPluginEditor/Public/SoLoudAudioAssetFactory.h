// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Factories/Factory.h"
#include "SoLoudAudioAssetFactory.generated.h"

/**
 * 
 */
UCLASS()
class SOLOUDPLUGINEDITOR_API USoLoudAudioAssetFactory : public UFactory
{
	GENERATED_BODY()	

public:
    USoLoudAudioAssetFactory();

    virtual UObject* FactoryCreateBinary(
        UClass* InClass,
        UObject* InParent,
        FName InName,
        EObjectFlags Flags,
        UObject* Context,
        const TCHAR* Type,
        const uint8*& Buffer,
        const uint8* BufferEnd,
        FFeedbackContext* Warn
    ) override;
};
