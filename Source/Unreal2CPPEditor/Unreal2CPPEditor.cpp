// Fill out your copyright notice in the Description page of Project Settings.

#include "Unreal2CPPEditor.h"
#include "Modules/ModuleManager.h"

IMPLEMENT_MODULE(FIUnreal2CPPEditorModule, Unreal2CPPEditor);

void FIUnreal2CPPEditorModule::StartupModule()
{
	UE_LOG(LogTemp, Warning, TEXT("Unreal2CPPEditorModule StartupModule() called"));
}

void FIUnreal2CPPEditorModule::ShutdownModule()
{
	UE_LOG(LogTemp, Warning, TEXT("Unreal2CPPEditorModule ShutdownModule() called"));
}
