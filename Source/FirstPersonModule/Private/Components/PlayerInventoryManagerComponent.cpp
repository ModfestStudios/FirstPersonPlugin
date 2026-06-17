// Fill out your copyright notice in the Description page of Project Settings.


#include "Components/PlayerInventoryManagerComponent.h"

#include "Inventory/InventoryEquipmentSlot.h"

/*items*/
#include "Equipment/Backpack.h"
#include "Clothing/Glasses.h"
#include "Clothing/Gloves.h"
#include "Clothing/Jacket.h"
#include "Equipment/Mask.h"
#include "Clothing/Pants.h"
#include "Clothing/Shirt.h"
#include "Clothing/Shoes.h"
#include "Clothing/Underwear.h"
#include "Clothing/Undershirt.h"
#include "Equipment/Vest.h"
#include "Equipment/Watch.h"
#include "Clothing/Headwear.h"

UPlayerInventoryManagerComponent::UPlayerInventoryManagerComponent()
{
	if (HasAnyFlags(RF_ClassDefaultObject))
	{
		
	}
	
	EquipmentSlots.Add(CreateDefaultEquipmentSlot("Headwear", AHeadwear::StaticClass()));
	EquipmentSlots.Add(CreateDefaultEquipmentSlot("Vest", AVest::StaticClass()));
	EquipmentSlots.Add(CreateDefaultEquipmentSlot("Backpack", ABackpack::StaticClass()));

	EquipmentSlots.Add(CreateDefaultEquipmentSlot("Glasses", AGlasses::StaticClass()));
	EquipmentSlots.Add(CreateDefaultEquipmentSlot("Gloves", AGloves::StaticClass()));

	EquipmentSlots.Add(CreateDefaultEquipmentSlot("Jacket", AJacket::StaticClass()));
	EquipmentSlots.Add(CreateDefaultEquipmentSlot("Shirt", AShirt::StaticClass()));
	EquipmentSlots.Add(CreateDefaultEquipmentSlot("Pants", APants::StaticClass()));
	EquipmentSlots.Add(CreateDefaultEquipmentSlot("Shoes", AShoes::StaticClass()));
	EquipmentSlots.Add(CreateDefaultEquipmentSlot("Undershirt", AUndershirt::StaticClass()));
	EquipmentSlots.Add(CreateDefaultEquipmentSlot("Underwear", AUnderwear::StaticClass()));
	
	

	//EquipmentSlots.Add(FInventoryEquipmentSlot("Backpack",ABackpack::StaticClass()));
	//EquipmentSlots.Add(FInventoryEquipmentSlot("Glasses",AGlasses::StaticClass()));
	//EquipmentSlots.Add(FInventoryEquipmentSlot("Mask", AMask::StaticClass()));

	//EquipmentSlots.Add(FInventoryEquipmentSlot("Jacket",AJacket::StaticClass()));		
	//EquipmentSlots.Add(FInventoryEquipmentSlot("Shirt", AShirt::StaticClass())); 
	//EquipmentSlots.Add(FInventoryEquipmentSlot("Pants",APants::StaticClass()));
	//
	//EquipmentSlots.Add(FInventoryEquipmentSlot("Shoes", AShoes::StaticClass()));
	//EquipmentSlots.Add(FInventoryEquipmentSlot("Undershirt", AUndershirt::StaticClass()));
	//EquipmentSlots.Add(FInventoryEquipmentSlot("Underwear", AUnderwear::StaticClass()));
	//EquipmentSlots.Add(FInventoryEquipmentSlot("Vets",AVest::StaticClass()));
	//EquipmentSlots.Add(FInventoryEquipmentSlot("Watch",AWatch::StaticClass()));
}
