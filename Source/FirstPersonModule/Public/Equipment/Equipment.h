// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Inventory/InventoryItem.h"
#include "Equipment.generated.h"

/**
 * 
 */
UCLASS(abstract)
class FIRSTPERSONMODULE_API AEquipment : public AInventoryItem
{
	GENERATED_BODY()
public:
    AEquipment(const FObjectInitializer& ObjectInitializer);
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Inventory")
        class UInventoryManagerComponent* InventoryManager;
public:


	
};
