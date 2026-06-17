// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Inventory/InventoryItem.h"
#include "USBStick.generated.h"

/**
 * 
 */
UCLASS()
class FIRSTPERSONMODULE_API AUSBStick : public AInventoryItem
{
	GENERATED_BODY()
public:



	//===================================================================================================================================
	//=============================================================FUNCTIONS=============================================================
	//===================================================================================================================================

	AUSBStick(const FObjectInitializer& ObjectInitializer);
	
};
