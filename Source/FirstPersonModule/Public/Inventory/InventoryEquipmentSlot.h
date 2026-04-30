// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "InventoryEquipmentSlot.generated.h"

/**
 * 
 */
UCLASS(BlueprintType,EditInlineNew,DefaultToInstanced, NotBlueprintable)
class FIRSTPERSONMODULE_API UInventoryEquipmentSlot : public UObject
{
	GENERATED_BODY()
public:


	UPROPERTY()
		class AActor* InventoryItem;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Equipment Slot")
		FName SlotID;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Equipment Slot", meta = (AllowAbstract = "true"))
		TSubclassOf<class AInventoryItem> ItemType;





	//=================================================================================================================================================
	//====================================================================FUNCTIONS====================================================================
	//=================================================================================================================================================
	
	UInventoryEquipmentSlot();



	UFUNCTION()
		bool IsItemEquipped();		

};
