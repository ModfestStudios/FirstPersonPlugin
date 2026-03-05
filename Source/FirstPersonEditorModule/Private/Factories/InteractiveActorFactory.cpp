// Fill out your copyright notice in the Description page of Project Settings.


#include "Factories/InteractiveActorFactory.h"
#include "Interactives/InteractiveActor.h"
#include "Kismet2/KismetEditorUtilities.h"
#include "FirstPersonEditorModule.h"

UInteractiveActorFactory::UInteractiveActorFactory()
{
    bCreateNew = true;
    bEditAfterNew = true;
    SupportedClass = UBlueprint::StaticClass();
}

UObject* UInteractiveActorFactory::FactoryCreateNew(UClass* Class, UObject* InParent, FName Name, EObjectFlags Flags, UObject* Context, FFeedbackContext* Warn)
{
    // Create a new Blueprint based on UInteractiveAction
    UBlueprint* NewBP = FKismetEditorUtilities::CreateBlueprint(
        AInteractiveActor::StaticClass(),
        InParent,
        Name,
        BPTYPE_Normal,
        UBlueprint::StaticClass(),
        UBlueprintGeneratedClass::StaticClass()
    );

    return NewBP;
}

FText UInteractiveActorFactory::GetDisplayName() const
{
    return NSLOCTEXT("AssetTypeActions", "FAssetTypeActions_InteractiveActor", "Interactive Actor");
}

uint32 UInteractiveActorFactory::GetMenuCategories() const
{
    return FFirstPersonEditorModule::InteractivesCategory;
}

FText UInteractiveActorFactory::GetToolTip() const
{
	return FText();
}
