// Copyright Epic Games, Inc. All Rights Reserved.

#include "SoLoudPluginEditor.h"

#include "AssetToolsModule.h"
#include "IAssetTools.h"
#include "SoLoudAssetActions.h"

#define LOCTEXT_NAMESPACE "FSoLoudPluginEditorModule"

void FSoLoudPluginEditorModule::StartupModule()
{
	// This code will execute after your module is loaded into memory; the exact timing is specified in the .uplugin file per-module
    // Registriamo le azioni dell'asset
    IAssetTools& AssetTools = FModuleManager::LoadModuleChecked<FAssetToolsModule>("AssetTools").Get();
    auto Action = MakeShared<FSoLoudAssetActions>();
    AssetTools.RegisterAssetTypeActions(Action);

    // Salviamo il riferimento per pulirlo allo shutdown se necessario
    RegisteredAssetActions.Add(Action);
}

void FSoLoudPluginEditorModule::ShutdownModule()
{
	// This function may be called during shutdown to clean up your module.  For modules that support dynamic reloading,
	// we call this function before unloading the module.
}

#undef LOCTEXT_NAMESPACE
	
IMPLEMENT_MODULE(FSoLoudPluginEditorModule, SoLoudPlugin)