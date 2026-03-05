// copyrighted 2024-2025 - Modfest Studios & Micah A. Parker


#include "UI/Components/Entries/InventoryItemAssetEntry.h"
#include "Inventory/InventoryItemAsset.h"
#include "Subsystems/InventorySubsystem.h"

void UInventoryItemAssetEntry::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);

	if (IsHovered() && bHoverState == false)
		Hover();
	else if (!IsHovered() && bHoverState == true)
		Unhover();
}

void UInventoryItemAssetEntry::NativeOnListItemObjectSet(UObject* ListItemObject)
{
	if (UInventoryItemAsset* Asset = Cast<UInventoryItemAsset>(ListItemObject))
		ItemAsset = Asset;

	/*call Blueprint version*/
	Execute_OnListItemObjectSet(Cast<UObject>(this), ListItemObject);
}

void UInventoryItemAssetEntry::Hover()
{
	bHoverState = true;

	if (UInventorySubsystem* ISS = GetWorld()->GetSubsystem<UInventorySubsystem>())
	{
		ISS->SetHoveredItemAsset(ItemAsset);
	}
}

void UInventoryItemAssetEntry::Unhover()
{
	bHoverState = false;

	if (UInventorySubsystem* ISS = GetWorld()->GetSubsystem<UInventorySubsystem>())
	{
		ISS->ClearHoveredItemAsset(ItemAsset);
	}
}

