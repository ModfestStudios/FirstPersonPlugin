// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Inventory/InventoryItem.h"
#include "Clothing.generated.h"

/**
 * 
 */
UCLASS(abstract)
class FIRSTPERSONMODULE_API AClothing : public AInventoryItem
{
	GENERATED_BODY()
public:


	UPROPERTY(VisibleAnywhere, Category = "Inventory Manager")
		class UInventoryManagerComponent* InventoryManager;



	//===========================================================================================================================================================================
	//=================================================================================FUNCTIONS=================================================================================
	//===========================================================================================================================================================================

	AClothing(const FObjectInitializer& ObjectInitializer);


};
