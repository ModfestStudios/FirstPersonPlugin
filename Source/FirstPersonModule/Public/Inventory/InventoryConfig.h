// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Inventory/Inventory.h"
#include "Engine/DataAsset.h"
#include "InventoryConfig.generated.h"


/** UInventoryConfig - Just a simple list of Item Classes that can be organized and used in loot tables or Loadout Selections
 * 
 */
UCLASS(BlueprintType)
class FIRSTPERSONMODULE_API UInventoryConfig : public UDataAsset
{
	GENERATED_BODY()
public:

	/*list of items*/
	UPROPERTY(EditDefaultsOnly, Category = "Inventory Items")
		TArray<TSoftObjectPtr<class UInventoryItemAsset>> Items;


	//UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Inventory")
	//	TSubclassOf<AActor> ItemClass;
	//UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Inventory", meta=(UIMin=1,ClampMin=1))
	//	int32 Quantity = 1;




	
};
