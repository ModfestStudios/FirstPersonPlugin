// Fill out your copyright notice in the Description page of Project Settings.


#include "Equipment/Equipment.h"


#include "Components/InventoryManagerComponent.h"

AEquipment::AEquipment(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
    InventoryManager = ObjectInitializer.CreateDefaultSubobject<UInventoryManagerComponent>(this, TEXT("Inventory Manager"));
    InventoryManager->SlotCapacity = 0; // Capacity belongs to the actual equipment asset configuration.
}
