// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/Components/Entries/InventoryManagerEntry.h"
#include "Components/InventoryManagerComponent.h"

void UInventoryManagerEntry::NativeOnListItemObjectSet(UObject* ListItemObject)
{
	if (UInventoryManagerComponent* InventoryManagerComp = Cast<UInventoryManagerComponent>(ListItemObject))
	{
		InventoryManager = InventoryManagerComp;
	}

	/*call Blueprint version*/
	Execute_OnListItemObjectSet(Cast<UObject>(this), ListItemObject);
}

FText UInventoryManagerEntry::GetInventoryManagerName()
{
	if (InventoryManager)
		return InventoryManager->GetInventoryManagerName();

	return FText();
}

UMaterialInterface* UInventoryManagerEntry::GetInventoryIcon()
{
	if (InventoryManager)
		return InventoryManager->GetInventoryIcon();

	return nullptr;
}
