// Fill out your copyright notice in the Description page of Project Settings.


#include "Factories/CharacterAssetFactory.h"
#include "FirstPersonModule/Public/Characters/CharacterAsset.h"
#include "FirstPersonEditorModule.h"

UCharacterAssetFactory::UCharacterAssetFactory()
{
	bCreateNew = true;
	bEditAfterNew = true;
	SupportedClass = UCharacterAsset::StaticClass();
}

UObject* UCharacterAssetFactory::FactoryCreateNew(UClass* Class, UObject* InParent, FName Name, EObjectFlags Flags, UObject* Context, FFeedbackContext* Warn)
{
	return NewObject<UCharacterAsset>(InParent, Class, Name, Flags);
}

FText UCharacterAssetFactory::GetDisplayName() const
{
	return NSLOCTEXT("AssetTypeActions", "FAssetTypeActions_CharacterAsset", "Character");
}

uint32 UCharacterAssetFactory::GetMenuCategories() const
{
	return FFirstPersonEditorModule::CharactersCategory;
}

FText UCharacterAssetFactory::GetToolTip() const
{
	return FText();
}
