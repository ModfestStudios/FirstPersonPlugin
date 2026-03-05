// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Info.h"
#include "Inventory/Inventory.h"
#include "Interfaces/InventoryItemInterface.h"
#include "InventoryLoadout.generated.h"

/**
 * 
 */
UCLASS()
class FIRSTPERSONMODULE_API AInventoryLoadout : public AInfo
{
	GENERATED_BODY()
public:

	UPROPERTY(BlueprintReadWrite, Category = "Inventory Loadout")
		FInventoryItemSpawnParams PrimaryWeapon;
	UPROPERTY(BlueprintReadWrite, Category = "Inventory Loadout")
		FInventoryItemSpawnParams SecondaryWeapon;
	UPROPERTY(BlueprintReadWrite, Category = "Inventory Loadout")
		FInventoryItemSpawnParams AlternativeWeapon;


	UPROPERTY(BlueprintReadWrite, Category = "Inventory Loadout")
		TArray<FInventoryItemSpawnParams> Items;

	//===============================================================================================================================
	//===========================================================FUNCTIONS===========================================================
	//===============================================================================================================================

	AInventoryLoadout();


	UFUNCTION(BlueprintCallable, Category = "Inventory Loadout")
		virtual void SetPrimaryWeapon(FInventoryItemSpawnParams InventoryItemSpawnParams);
	UFUNCTION(BlueprintCallable, Category = "Inventory Loadout")
		virtual void ClearPrimaryWeapon();
	UFUNCTION(BlueprintCallable, Category = "Inventory Loadout")
		virtual void SetSecondaryWeapon(FInventoryItemSpawnParams InventoryItemSpawnParams);
	UFUNCTION(BlueprintCallable, Category = "Inventory Loadout")
		virtual void ClearSecondaryWeapon();

	UFUNCTION(BlueprintCallable, Category = "Inventory Loadout")
		virtual void SetAlternativeWeapon(FInventoryItemSpawnParams InventoryItemSpawnParams);
	UFUNCTION(BlueprintCallable, Category = "Inventory Loadout")
		virtual void ClearAlternativeWeapon();

};
