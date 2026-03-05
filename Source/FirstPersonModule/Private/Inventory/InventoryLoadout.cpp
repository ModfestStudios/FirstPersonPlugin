// Fill out your copyright notice in the Description page of Project Settings.


#include "Inventory/InventoryLoadout.h"

AInventoryLoadout::AInventoryLoadout()
{	
	bReplicates = true;
	bAlwaysRelevant = true;
}

void AInventoryLoadout::SetPrimaryWeapon(FInventoryItemSpawnParams InventoryItemSpawnParams)
{
	PrimaryWeapon = InventoryItemSpawnParams;
}

void AInventoryLoadout::ClearPrimaryWeapon()
{
	PrimaryWeapon = FInventoryItemSpawnParams();
}

void AInventoryLoadout::SetSecondaryWeapon(FInventoryItemSpawnParams InventoryItemSpawnParams)
{
	SecondaryWeapon = InventoryItemSpawnParams;
}

void AInventoryLoadout::ClearSecondaryWeapon()
{
	SecondaryWeapon = FInventoryItemSpawnParams();
}

void AInventoryLoadout::SetAlternativeWeapon(FInventoryItemSpawnParams InventoryItemSpawnParams)
{
	AlternativeWeapon = InventoryItemSpawnParams;
}

void AInventoryLoadout::ClearAlternativeWeapon()
{
	AlternativeWeapon = FInventoryItemSpawnParams();
}
