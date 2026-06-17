// Fill out your copyright notice in the Description page of Project Settings.


#include "Inventory/InventoryItemSlot.h"
#include "Inventory/InventoryItem.h"
#include "Components/InventoryItemComponent.h"

/*networking*/
#include "Net/UnrealNetwork.h"


void UInventoryItemSlot::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	

	DOREPLIFETIME(UInventoryItemSlot, InventoryItem);
	DOREPLIFETIME(UInventoryItemSlot, ItemClass);
	
}

FText UInventoryItemSlot::GetItemName() const
{
	if (InventoryItem.IsValid())
	{
		return InventoryItem.Get()->GetItemName();
	}

	if (ItemClass)
	{		
		if (AInventoryItem* ItemDefault = ItemClass->GetDefaultObject<AInventoryItem>())
			return ItemDefault->GetItemName();
	}
	
	return FText();
}

UMaterialInterface* UInventoryItemSlot::GetItemIcon() const
{
	if (InventoryItem.IsValid())
		return InventoryItem.Get()->GetItemIcon();
	
	if (ItemClass)
	{
		if (AInventoryItem* ItemDefault = ItemClass->GetDefaultObject<AInventoryItem>())
			return ItemDefault->GetItemIcon();
	}
	
	return nullptr;
}

EItemSize UInventoryItemSlot::GetItemSize() const
{
	if (InventoryItem.IsValid())
		return InventoryItem->GetItemSize();
	
	if (ItemClass)
	{
		if (AInventoryItem* ItemDefault = ItemClass->GetDefaultObject<AInventoryItem>())
		{
			return ItemDefault->GetItemSize();
		}
	}

	return EItemSize::Small;
}

int32 UInventoryItemSlot::GetItemSlotSize() const
{
	switch (GetItemSize())
	{
		case EItemSize::ExtraLarge:
			return INVENTORY_SLOT_SIZE_XLG;
		case EItemSize::Large:
			return INVENTORY_SLOT_SIZE_LG;
		case EItemSize::Medium:
			return INVENTORY_SLOT_SIZE_MD;
		case EItemSize::Small:
			return INVENTORY_SLOT_SIZE_SM;
		default:
			return INVENTORY_SLOT_SIZE_SM;
	}
}

void UInventoryItemSlot::SetOwner(UInventoryManagerComponent* NewOwner)
{
	Owner = NewOwner;
}

UInventoryManagerComponent* UInventoryItemSlot::GetOwner()
{
	return Owner;
}

void UInventoryItemSlot::OnAddedToInventoryManager(UInventoryManagerComponent* InventoryManager)
{
	if (!InventoryManager)
		return;

	
}

void UInventoryItemSlot::OnRemovedFromInventoryManager(UInventoryManagerComponent* InventoryManager)
{
	if (!InventoryManager)
		return;



}
