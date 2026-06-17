// Fill out your copyright notice in the Description page of Project Settings.


#include "Clothing/Clothing.h"

/*components*/
#include "Components/InventoryItemComponent.h"
#include "Components/InventoryManagerComponent.h"

AClothing::AClothing(const FObjectInitializer& ObjectInitializer)
	:Super(ObjectInitializer)
{

	InventoryManager = ObjectInitializer.CreateDefaultSubobject<UInventoryManagerComponent>(this, TEXT("Inventory Manager"));
}
