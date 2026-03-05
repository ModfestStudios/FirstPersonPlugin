// Fill out your copyright notice in the Description page of Project Settings.


#include "Factories/InteractiveActionFactory.h"
#include "Kismet2/KismetEditorUtilities.h"
#include "FirstPersonEditorModule.h"
#include "FirstPersonModule/Public/Interactives/InteractiveAction.h"

UInteractiveActionFactory::UInteractiveActionFactory()
{
	bCreateNew = true;
	bEditAfterNew = true;
	SupportedClass = UBlueprint::StaticClass();
}

UObject* UInteractiveActionFactory::FactoryCreateNew(UClass* Class, UObject* InParent, FName Name, EObjectFlags Flags, UObject* Context, FFeedbackContext* Warn)
{
    // Create a new Blueprint based on UInteractiveAction
    UBlueprint* NewBP = FKismetEditorUtilities::CreateBlueprint(
        UInteractiveAction::StaticClass(),
        InParent,
        Name,
        BPTYPE_Normal,
        UBlueprint::StaticClass(),
        UBlueprintGeneratedClass::StaticClass()
    );

    return NewBP;    
}

FText UInteractiveActionFactory::GetDisplayName() const
{
    return NSLOCTEXT("AssetTypeActions", "FAssetTypeActions_InteractiveAction", "Interactive Action");

}

uint32 UInteractiveActionFactory::GetMenuCategories() const
{
    //return EAssetTypeCategories::Gameplay;
    return FFirstPersonEditorModule::InteractivesCategory;
}

FText UInteractiveActionFactory::GetToolTip() const
{
    return FText::FromString("Interactive Actions allow Blueprint Scriptable actions that can be attached to InteractiveComponents giving new Interaction options");
}
