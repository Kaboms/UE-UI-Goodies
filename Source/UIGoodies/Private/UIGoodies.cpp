// Copyright Epic Games, Inc. All Rights Reserved.

#include "UIGoodies.h"
#include "RadialMenu/RadialMenuDetails.h"

#define LOCTEXT_NAMESPACE "FUIGoodiesModule"

void FUIGoodiesModule::StartupModule()
{
	// Class detail customizations
	FPropertyEditorModule& PropertyModule = FModuleManager::GetModuleChecked<FPropertyEditorModule>(TEXT("PropertyEditor"));
	PropertyModule.RegisterCustomClassLayout(TEXT("RadialMenu"), FOnGetDetailCustomizationInstance::CreateStatic(&FRadialMenuDetails::MakeInstance));
}

void FUIGoodiesModule::ShutdownModule()
{
	// This function may be called during shutdown to clean up your module.  For modules that support dynamic reloading,
	// we call this function before unloading the module.
}

#undef LOCTEXT_NAMESPACE
	
IMPLEMENT_MODULE(FUIGoodiesModule, UIGoodies)