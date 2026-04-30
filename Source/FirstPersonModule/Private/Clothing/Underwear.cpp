// Fill out your copyright notice in the Description page of Project Settings.


#include "Clothing/Underwear.h"

/*components*/
#include "Components/InventoryManagerComponent.h"
#include "Components/InventoryItemComponent.h"


/*inventory*/
#include "Inventory/Inventory.h"

AUnderwear::AUnderwear(const FObjectInitializer& ObjectInitializer)
	:Super(ObjectInitializer)
{
	if (InventoryManager)
	{
		InventoryManager->SlotCapacity = 1;		
		InventoryManager->InventoryManagerName = NSLOCTEXT("Inventory", "UnderwearContainerName", "Underwear");
	}
	if (ItemComponent)
	{
		ItemComponent->Size = EItemSize::Small;
	}
}
