// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "Modules/ModuleManager.h"
#include "IAssetTypeActions.h"

class FSoLoudPluginEditorModule : public IModuleInterface
{
public:

	/** IModuleInterface implementation */
	virtual void StartupModule() override;
	virtual void ShutdownModule() override;

private:
	/** Conserva i riferimenti alle azioni registrate per poterle pulire allo spegnimento */
	TArray<TSharedPtr<IAssetTypeActions>> RegisteredAssetActions;
};
