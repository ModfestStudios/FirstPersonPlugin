// Fill out your copyright notice in the Description page of Project Settings.

#include "FirstPersonEditorModule.h"
#include "Logging.h"
#include "AssetToolsModule.h"
#include "CoreMinimal.h"
#include "AssetTypeActions_Base.h"
#include "FirstPersonModule/Public/Interactives/InteractiveAction.h"
#include "FirstPersonModule/Public/Characters/CharacterInfo.h"
#include "FirstPersonModule/Public/Missions/MissionAsset.h"
#include "Modules/ModuleManager.h"

#define LOCTEXT_NAMESPACE "FFirstPersonEditorModule"

EAssetTypeCategories::Type FFirstPersonEditorModule::FirstPersonCategory;
EAssetTypeCategories::Type FFirstPersonEditorModule::InventoryCategory;
EAssetTypeCategories::Type FFirstPersonEditorModule::InteractivesCategory;
EAssetTypeCategories::Type FFirstPersonEditorModule::CharactersCategory;
EAssetTypeCategories::Type FFirstPersonEditorModule::MissionsCategory;
EAssetTypeCategories::Type FFirstPersonEditorModule::MapsCategory;
//
//class FAssetTypeActions_InteractiveAction : public FAssetTypeActions_Base
//{
//public:
//    FAssetTypeActions_InteractiveAction(EAssetTypeCategories::Type InCategory)
//        : MyAssetCategory(InCategory) {}
//
//    // Asset type this action applies to
//    virtual UClass* GetSupportedClass() const override
//    {
//        return UInteractiveAction::StaticClass();
//    }
//
//    // Name of the asset type
//    virtual FText GetName() const override
//    {
//        return NSLOCTEXT("AssetTypeActions", "FAssetTypeActions_InteractiveAction", "Interactive Action");
//    }
//
//    // Category for the asset
//    virtual uint32 GetCategories() override
//    {
//        return MyAssetCategory;
//    }
//
//    // Optional: Customize icon for the asset
//    virtual FColor GetTypeColor() const override
//    {
//        return FColor::Orange;
//    }
//
//private:
//    EAssetTypeCategories::Type MyAssetCategory;
//};


class FAssetTypeActions_Missions : public FAssetTypeActions_Base
{
public:
    FAssetTypeActions_Missions(EAssetTypeCategories::Type InCategory)
        : MyAssetCategory(InCategory) {
    }

    // Asset type this action applies to
    virtual UClass* GetSupportedClass() const override
    {
        return UMissionAsset::StaticClass();
    }

    // Name of the asset type
    virtual FText GetName() const override
    {
        return NSLOCTEXT("AssetTypeActions", "FAssetTypeActions_Missions", "Mission");
    }

    // Category for the asset
    virtual uint32 GetCategories() override
    {
        return MyAssetCategory;
    }

    // Optional: Customize icon for the asset
    virtual FColor GetTypeColor() const override
    {
        return FColor::Green;
    }

private:
    EAssetTypeCategories::Type MyAssetCategory;
};

class FAssetTypeActions_Characters : public FAssetTypeActions_Base
{
public:
    FAssetTypeActions_Characters(EAssetTypeCategories::Type InCategory)
        : MyAssetCategory(InCategory) {
    }

    // Asset type this action applies to
    virtual UClass* GetSupportedClass() const override
    {
        return ACharacterInfo::StaticClass();
    }

    // Name of the asset type
    virtual FText GetName() const override
    {
        return NSLOCTEXT("AssetTypeActions", "FAssetTypeActions_Characters", "Characters");
    }

    // Category for the asset
    virtual uint32 GetCategories() override
    {
        return MyAssetCategory;
    }

    // Optional: Customize icon for the asset
    virtual FColor GetTypeColor() const override
    {
        return FColor::Cyan;
    }

private:
    EAssetTypeCategories::Type MyAssetCategory;
};


void FFirstPersonEditorModule::StartupModule()
{
    IAssetTools& AssetTools = FModuleManager::LoadModuleChecked<FAssetToolsModule>("AssetTools").Get();

    // Register a custom asset category
    FirstPersonCategory = AssetTools.RegisterAdvancedAssetCategory(
        FName(TEXT("First Person Module")),  // Category identifier
        FText::FromString("First Person Module")  // Display name
    );
    InventoryCategory = AssetTools.RegisterAdvancedAssetCategory(
        FName(TEXT("Inventory")),  // Category identifier
        FText::FromString("Inventory")  // Display name
    );
    InteractivesCategory = AssetTools.RegisterAdvancedAssetCategory(
        FName(TEXT("Interactives")),  // Category identifier
        FText::FromString("Interactives")  // Display name
    );
    CharactersCategory = AssetTools.RegisterAdvancedAssetCategory(
        FName(TEXT("Characters")),
        FText::FromString("Characters")
    );
    MapsCategory = AssetTools.RegisterAdvancedAssetCategory(
        FName(TEXT("Maps")),
        FText::FromString("Maps")
    );
    MissionsCategory = AssetTools.RegisterAdvancedAssetCategory(
        FName(TEXT("Missions")),
        FText::FromString("Missions")
    );

    

    //// Register the factory or asset actions here
    //AssetTools.RegisterAssetTypeActions(
    //    MakeShareable(new FAssetTypeActions_InteractiveAction(InteractiveActionCategory))
    //);
    AssetTools.RegisterAssetTypeActions(
        MakeShareable(new FAssetTypeActions_Missions(MissionsCategory)));

    AssetTools.RegisterAssetTypeActions(
        MakeShareable(new FAssetTypeActions_Characters(CharactersCategory))
    );
}

void FFirstPersonEditorModule::ShutdownModule()
{
    /*if (FModuleManager::Get().IsModuleLoaded("AssetTools"))
    {
        IAssetTools& AssetTools = FModuleManager::GetModuleChecked<FAssetToolsModule>("AssetTools");
        AssetTools.UnregisterAssetTypeActions(InteractiveActionCategory);
    }*/
}

#undef LOCTEXT_NAMESPACE
	
IMPLEMENT_MODULE(FFirstPersonEditorModule, FirstPersonEditorModule);