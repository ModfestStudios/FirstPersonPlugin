// Fill out your copyright notice in the Description page of Project Settings.


#include "Factories/CharacterInfoFactory.h"
#include "Kismet2/KismetEditorUtilities.h"
#include "FirstPersonModule/Public/Characters/CharacterInfo.h"
#include "FirstPersonEditorModule.h"

UCharacterInfoFactory::UCharacterInfoFactory()
{
	bCreateNew = true;
	bEditAfterNew = true;
	SupportedClass = ACharacterInfo::StaticClass();
}

UObject* UCharacterInfoFactory::FactoryCreateNew(UClass* Class, UObject* InParent, FName Name, EObjectFlags Flags, UObject* Context, FFeedbackContext* Warn)
{
    // Create a new Blueprint based on UInteractiveAction
    UBlueprint* NewBP = FKismetEditorUtilities::CreateBlueprint(
        ACharacterInfo::StaticClass(),
        InParent,
        Name,
        BPTYPE_Normal,
        UBlueprint::StaticClass(),
        UBlueprintGeneratedClass::StaticClass()
    );

    return NewBP;
}

FText UCharacterInfoFactory::GetDisplayName() const
{
	return NSLOCTEXT("AssetTypeActions", "FAssetTypeActions_CharacterInfo", "Character Info");
}

uint32 UCharacterInfoFactory::GetMenuCategories() const
{
	return FFirstPersonEditorModule::CharactersCategory;
}

FText UCharacterInfoFactory::GetToolTip() const
{
	return FText();
}
