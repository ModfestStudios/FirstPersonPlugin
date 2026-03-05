// copyrighted 2024-2025 - Modfest Studios & Micah A. Parker


#include "Subsystems/InventorySubsystem.h"
#include "Inventory/InventoryItemAsset.h"
#include "Components/InventoryItemComponent.h"
#include "Components/InventoryManagerComponent.h"

UInventoryItemAsset* UInventorySubsystem::GetHoveredItemAsset()
{
	return HoveredItemAsset;
}

void UInventorySubsystem::SetHoveredItemAsset(UInventoryItemAsset* ItemAsset)
{
	HoveredItemAsset = ItemAsset;
}

void UInventorySubsystem::ClearHoveredItemAsset(UInventoryItemAsset* ItemAsset)
{	
	/*if no ItemAsset is passed - just clear*/
	if (ItemAsset == nullptr)
		HoveredItemAsset = nullptr;

	/*if we passed an ItemAsset - confirm the hovered one is the same and clear if we find a match*/
	if (ItemAsset == HoveredItemAsset)
		HoveredItemAsset = nullptr;

}

UInventoryItemComponent* UInventorySubsystem::GetInventoryItemComponent(AActor* Actor)
{
	if (!Actor)
		return nullptr;

	return Actor->GetComponentByClass<UInventoryItemComponent>();
}
