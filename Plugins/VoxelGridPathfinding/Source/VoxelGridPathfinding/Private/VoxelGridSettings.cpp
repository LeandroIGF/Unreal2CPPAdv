
#include "VoxelGridSettings.h"

UVoxelGridSettings::UVoxelGridSettings()
{
	CategoryName = TEXT("Plugins");
	SectionName = TEXT("Voxel Grid Pathfinding");
}


/*

FPath::ProjectSaveDir() + TEXT("SaveGames/")

FFileHelper::SaveStringToFile(String, *Path)

FFileHelper::LoadFileToString(String, *fullpath)

IPlatformFile::GetPlatformFile().CreateDirectoryTree(*Path)

#include "JsonObjectConverter.h" 
#include "Misc/FileHelper.h" 
#include "Misc/Paths.h"

FPlatformProcess::Sleep(2.0f)

*/