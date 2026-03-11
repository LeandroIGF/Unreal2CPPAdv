// Copyright Epic Games, Inc. All Rights Reserved.

using System.IO;
using UnrealBuildTool;

public class SoLoudPlugin : ModuleRules
{
	public SoLoudPlugin(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = ModuleRules.PCHUsageMode.UseExplicitOrSharedPCHs;

        string SoLoudPath = Path.Combine(ModuleDirectory, "../ThirdParty/SoLoud");

		string Sdl2Path = Path.Combine(ModuleDirectory, "../ThirdParty/SDL2-2.30.0");

        PublicIncludePaths.AddRange(
			new string[] {
				// ... add public include paths required here ...
				Path.Combine(Sdl2Path, "include"),
			}
			);
				
		
		PrivateIncludePaths.AddRange(
			new string[] {
				// ... add other private include paths required here ...
				Path.Combine(SoLoudPath, "include"),
				Path.Combine(SoLoudPath, "src/core"),
				Path.Combine(SoLoudPath, "src/audiosource/wav"),
				Path.Combine(SoLoudPath, "src/backend/sdl2_static"),
            }
		);		
		
		
		// AddEngineThirdPartyPrivateStaticDependencies(Target, "SDL2");
		PublicAdditionalLibraries.Add(Path.Combine(Sdl2Path, "lib", "x64", "SDL2.lib"));
		RuntimeDependencies.Add("$(PluginDir)/Binaries/Win64/SDL2.dll");

		PublicDefinitions.Add("WITH_SDL2_STATIC=1");
		PublicDefinitions.Add("SOLOUD_DLL_EXPORT=1");

        // Disabilitiamo i warning che bloccherebbero la build
		CppCompileWarningSettings.UndefinedIdentifierWarningLevel = WarningLevel.Off;
		CppCompileWarningSettings.ShadowVariableWarningLevel = WarningLevel.Off;

        PublicDependencyModuleNames.AddRange(
			new string[]
			{
				"Core",
                //"SDL2"
				// ... add other public dependencies that you statically link with here ...
			}
		);
			
		
		PrivateDependencyModuleNames.AddRange(
			new string[]
			{
				"CoreUObject",
				"Engine",
				"Slate",
				"SlateCore",
				// ... add private dependencies that you statically link with here ...	
			}
			);
		
		
		DynamicallyLoadedModuleNames.AddRange(
			new string[]
			{
				// ... add any modules that your module loads dynamically here ...
			}
			);
	}
}
