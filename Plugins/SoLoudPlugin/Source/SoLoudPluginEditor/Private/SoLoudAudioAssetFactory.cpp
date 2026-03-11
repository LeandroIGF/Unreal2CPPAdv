// Fill out your copyright notice in the Description page of Project Settings.


#include "SoLoudAudioAssetFactory.h"
#include "SoLoudAudioAsset.h"
#include "SoLoudSFXAsset.h"

USoLoudAudioAssetFactory::USoLoudAudioAssetFactory()
{
    SupportedClass = USoLoudAudioAsset::StaticClass();
	//SupportedClass = USoLoudSFXAsset::StaticClass(); // Se vuoi importare direttamente in SFXAsset
    bCreateNew = false;
    bEditorImport = true;

    // Estensione custom per evitare il conflitto con l'importatore WAV di default
    //Formats.Add(TEXT("slwav;SoLoud Wave Audio File"));

    // Percorso B (Forza Bruta): Ruba l'estensione wav ad Unreal
   Formats.Add(TEXT("wav;SoLoud Wave Audio File"));
   ImportPriority = DefaultImportPriority + 100; // Ti assicura che scatti prima del Factory nativo

}

UObject* USoLoudAudioAssetFactory::FactoryCreateBinary(
    UClass* InClass,
    UObject* InParent,
    FName InName,
    EObjectFlags Flags,
    UObject* Context,
    const TCHAR* Type,
    const uint8*& Buffer,
    const uint8* BufferEnd,
    FFeedbackContext* Warn)
{
    USoLoudAudioAsset* NewAudioAsset = NewObject<USoLoudAudioAsset>(InParent, InClass, InName, Flags);
    //USoLoudSFXAsset* NewAudioAsset = NewObject<USoLoudSFXAsset>(InParent, InClass, InName, Flags);
    if (NewAudioAsset)
    {
        int32 BytesSize = BufferEnd - Buffer;
        NewAudioAsset->RawAudioData.Empty(BytesSize);
        NewAudioAsset->RawAudioData.Append(Buffer, BytesSize);
		//NewAudioAsset->AssetID = TEXT("Hello"); // Puoi calcolare la durata se necessario, altrimenti lasciala a 0 o a un valore di default
    }

    return NewAudioAsset;
}