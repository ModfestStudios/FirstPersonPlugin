// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AssetToolsModule.h"
#include "Modules/ModuleManager.h"

class FFirstPersonEditorModule : public IModuleInterface
{
public:

	static EAssetTypeCategories::Type FirstPersonCategory;
	static EAssetTypeCategories::Type InventoryCategory;
	static EAssetTypeCategories::Type InteractivesCategory;
	static EAssetTypeCategories::Type CharactersCategory;
	static EAssetTypeCategories::Type MissionsCategory;
	static EAssetTypeCategories::Type MapsCategory;

	/** IModuleInterface implementation */
	void StartupModule() override;
	void ShutdownModule() override;
	
};
