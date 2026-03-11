// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "AssetTypeActions_Base.h"
#include "SoLoudAudioAsset.h"


class FSoLoudAssetActions : public FAssetTypeActions_Base
{
public:
    // Nome visualizzato nel menu "Create Asset"
    virtual FText GetName() const override { return INVTEXT("SoLoud Audio Asset"); }

    // Colore della barra sotto l'icona (es. un arancione acceso)
    virtual FColor GetTypeColor() const override { return FColor(255, 128, 0); }

    // Associa questa azione alla nostra classe asset
    virtual UClass* GetSupportedClass() const override { return USoLoudAudioAsset::StaticClass(); }

    // Categoria in cui apparirà (es. Sounds)
    virtual uint32 GetCategories() override { return EAssetTypeCategories::Sounds; }

    virtual const FSlateBrush* GetThumbnailBrush(const FAssetData& InAssetData, const FName InClassName) const override
    {
        // Di solito per le miniature si usa lo stesso brush dell'icona
        // più grande se esistesse (es. "ClassThumbnail.SoundWave")
        return FAppStyle::GetBrush("ClassIcon.SoundWave");
    }
};
