
#pragma once

#include "CoreMinimal.h"
#include "Engine/DeveloperSettings.h"
#include "VoxelGridSettings.generated.h"

/**
 * Settings for Voxel Grid Pathfinding Plugin
 */
UCLASS(Config=Game, defaultconfig, meta = (DisplayName = "Voxel Grid Pathfinding"))
class VOXELGRIDPATHFINDING_API UVoxelGridSettings : public UDeveloperSettings
{
	GENERATED_BODY()

public:
	UVoxelGridSettings();

	/** Dimension of each voxel cube (e.g. 100.0) */
	UPROPERTY(EditAnywhere, Config, Category = "General")
	float VoxelSize = 100.f;

	/** Max pathfinding/generation requests to process per tick */
	UPROPERTY(EditAnywhere, Config, Category = "Performance")
	int32 MaxRequestsPerTick = 5;

	/** Max pathfinding/generation requests to process per tick */
	UPROPERTY(EditAnywhere, Config, Category = "Performance")
	int32 MaxIters = 300000;

	/** Show debug visualization for Voxel Grid generation (Free/Blocked) */
	UPROPERTY(EditAnywhere, Config, Category = "Debug")
	bool bShowVoxelDebug = true;

	/** Show debug visualization for Pathfinding (Blue Line) */
	UPROPERTY(EditAnywhere, Config, Category = "Debug")
	bool bShowPathfindingDebug = true;

};
