// Fill out your copyright notice in the Description page of Project Settings.


#include "Factories/MissionFactory.h"
#include "FirstPersonModule/Public/Missions/MissionAsset.h"
#include "FirstPersonEditorModule.h"

UMissionFactory::UMissionFactory()
{
	bCreateNew = true;
	bEditAfterNew = true;
	SupportedClass = UMissionAsset::StaticClass();
}

UObject* UMissionFactory::FactoryCreateNew(UClass* Class, UObject* InParent, FName Name, EObjectFlags Flags, UObject* Context, FFeedbackContext* Warn)
{
	return NewObject<UMissionAsset>(InParent, Class, Name, Flags);
}

FText UMissionFactory::GetDisplayName() const
{
	return NSLOCTEXT("AssetTypeActions", "FAssetTypeActions_MissionAsset", "Mission");
}

uint32 UMissionFactory::GetMenuCategories() const
{
	return FFirstPersonEditorModule::MissionsCategory;
}

FText UMissionFactory::GetToolTip() const
{
	return FText();
}
