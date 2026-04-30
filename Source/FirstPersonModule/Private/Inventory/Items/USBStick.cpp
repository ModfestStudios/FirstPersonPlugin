// Fill out your copyright notice in the Description page of Project Settings.


#include "Inventory/Items/USBStick.h"
#include "Components/InventoryItemComponent.h"

AUSBStick::AUSBStick(const FObjectInitializer& ObjectInitializer)
:Super(ObjectInitializer)
{

	if (ItemComponent)
	{
		ItemComponent->ItemName = NSLOCTEXT("Inventory Items", "USB", "USB Stick");
		ItemComponent->Size = EItemSize::Small;
	}
}
