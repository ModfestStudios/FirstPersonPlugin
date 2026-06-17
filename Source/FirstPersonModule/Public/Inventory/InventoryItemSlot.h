// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Inventory/Inventory.h"
#include "UObject/NoExportTypes.h"
#include "InventoryItemSlot.generated.h"

/**
 * 
 */
UCLASS(BlueprintType)
class FIRSTPERSONMODULE_API UInventoryItemSlot : public UObject
{
	GENERATED_BODY()
public:


	/*reference to inventory item if it's spawned*/
	UPROPERTY(Replicated, BlueprintReadOnly, Category = "Inventory Slot")
		TWeakObjectPtr<class AInventoryItem> InventoryItem;
	/*the item class this represents*/
	UPROPERTY(Replicated, BlueprintReadOnly, Category = "Inventory Slot")
		TSubclassOf<class AInventoryItem> ItemClass;	
protected:
	UPROPERTY(Replicated, BlueprintReadOnly, Category = "Inventory Slot")
		class UInventoryManagerComponent* Owner = nullptr;

	//===========================================================================================================================================================
	//=========================================================================FUNCTIONS=========================================================================
	//===========================================================================================================================================================

public:

	/*enable replication on UObject*/
	virtual bool IsSupportedForNetworking() const override { return true; }

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;


public:
	UFUNCTION(BlueprintPure, Category = "Inventory Slot")
		FText GetItemName() const;
	UFUNCTION(BlueprintPure, Category = "Inventory Slot")
		class UMaterialInterface* GetItemIcon() const;
	UFUNCTION(BlueprintPure, Category = "Inventory Slot")
		EItemSize GetItemSize() const;
	UFUNCTION(BlueprintPure, Category = "Inventory Slot")
		int32 GetItemSlotSize() const;


public:
	UFUNCTION()
		virtual void SetOwner(class UInventoryManagerComponent* NewOwner);
	UFUNCTION()
		virtual class UInventoryManagerComponent* GetOwner();
	UFUNCTION()
		virtual void OnAddedToInventoryManager(class UInventoryManagerComponent* InventoryManager);
	UFUNCTION()
		virtual void OnRemovedFromInventoryManager(class UInventoryManagerComponent* InventoryManager);

};
