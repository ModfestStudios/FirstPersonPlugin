// copyrighted 2024-2025 - Modfest Studios & Micah A. Parker


#include "Factories/InventoryItemAssetFactory.h"
#include "Inventory/InventoryItemAsset.h"
#include "FirstPersonEditorModule.h"

UInventoryItemAssetFactory::UInventoryItemAssetFactory()
{
	bCreateNew = true;
	bEditAfterNew = true;
	SupportedClass = UInventoryItemAsset::StaticClass();
}

UObject* UInventoryItemAssetFactory::FactoryCreateNew(UClass* Class, UObject* InParent, FName Name, EObjectFlags Flags, UObject* Context, FFeedbackContext* Warn)
{
	return NewObject<UInventoryItemAsset>(InParent, Class, Name, Flags);
}

FText UInventoryItemAssetFactory::GetDisplayName() const
{
	return NSLOCTEXT("AssetTypeActions", "FAssetTypeActions_InventoryItemAsset", "Inventory Item Asset");
}

uint32 UInventoryItemAssetFactory::GetMenuCategories() const
{
	return FFirstPersonEditorModule::InventoryCategory;
}

FText UInventoryItemAssetFactory::GetToolTip() const
{
	return FText();
}
