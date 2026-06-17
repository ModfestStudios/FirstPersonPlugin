// Fill out your copyright notice in the Description page of Project Settings.


#include "Inventory/InventoryEquipmentSlot.h"


UInventoryEquipmentSlot::UInventoryEquipmentSlot()
{
}

bool UInventoryEquipmentSlot::IsItemEquipped()
{
	if (InventoryItem != nullptr)
		return true;
	else
		return false;
}
