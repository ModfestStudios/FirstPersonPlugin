// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Inventory/InventoryItem.h"
#include "Clothing/Clothing.h"
#include "Underwear.generated.h"

/**
 * 
 */
UCLASS(abstract)
class FIRSTPERSONMODULE_API AUnderwear : public AClothing
{
	GENERATED_BODY()
public:







	//=======================================================================================================================================================================
	//===============================================================================FUNCTIONS===============================================================================
	//=======================================================================================================================================================================

	AUnderwear(const FObjectInitializer& ObjectInitializer);

	
};
