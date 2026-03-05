// Fill out your copyright notice in the Description page of Project Settings.


#include "Factories/MapAssetFactory.h"
#include "FirstPersonModule/Public/Maps/MapAsset.h"
#include "FirstPersonEditorModule.h"


UMapAssetFactory::UMapAssetFactory()
{
	bCreateNew = true;
	bEditAfterNew = true;
	SupportedClass = UMapAsset::StaticClass();
}

UObject* UMapAssetFactory::FactoryCreateNew(UClass* Class, UObject* InParent, FName Name, EObjectFlags Flags, UObject* Context, FFeedbackContext* Warn)
{
	return NewObject<UMapAsset>(InParent, Class, Name, Flags);
}

FText UMapAssetFactory::GetDisplayName() const
{
	return NSLOCTEXT("AssetTypeActions", "FAssetTypeActions_MapAssets", "Map");

}

uint32 UMapAssetFactory::GetMenuCategories() const
{
	return FFirstPersonEditorModule::MapsCategory;
}

FText UMapAssetFactory::GetToolTip() const
{
	return FText();
}
