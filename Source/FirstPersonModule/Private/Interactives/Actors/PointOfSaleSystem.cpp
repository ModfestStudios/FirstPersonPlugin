// Fill out your copyright notice in the Description page of Project Settings.


#include "Interactives/Actors/PointOfSaleSystem.h"
#include "Subsystems/InventorySubsystem.h"
#include "Components/InventoryItemComponent.h"




bool APointOfSaleSystem::ScanItem(AActor* Item)
{
	if (!IsValid(Item))
		return false;

	if (UInventoryItemComponent* ItemComp = Item->GetComponentByClass<UInventoryItemComponent>())
	{
		if (ItemComp->IsSellable())
		{
			ScannedItems.Add(Item);
			return true;
		}
		else
			return false;
	}
	else
		return false;	
}

bool APointOfSaleSystem::RemoveItem(AActor* Item)
{
	return false;
}
