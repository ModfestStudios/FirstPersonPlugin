// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Components/InventoryItemComponent.h"
#include "InventoryItemList.generated.h"

/**
 * 
 */
UCLASS()
class FIRSTPERSONMODULE_API UInventoryItemList : public UUserWidget
{
	GENERATED_BODY()
public:

protected:
	
	/*if true - will list all items in the vicinity of the manager rather than its own storage*/
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Inventory Items")
		bool bListItemsInVicinity = false;
	UPROPERTY(BlueprintReadOnly, Category = "Inventory Items")
		class UInventoryManagerComponent* InventoryManager;


	UPROPERTY()
		TArray<class UInventoryItemSlot*> InventorySlots;

	//=======================================================================================================================================
	//===============================================================FUNCTIONS===============================================================
	//=======================================================================================================================================


	virtual void NativeConstruct() override;

	UFUNCTION(BlueprintCallable, Category = "Inventory Items")
		virtual void SetInventoryManagerComponent(class UInventoryManagerComponent* NewInventoryManager, bool bSetListVicinityItems = false);
	UFUNCTION()
		void OnInventoryItemListUpdated();
	UFUNCTION(BlueprintImplementableEvent, Category = "Inventory Items", meta = (DisplayName = "On Inventory Item List Updated"))
		void BP_OnInventoryItemListUpdated();

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Inventory Slots")
		const TArray<class UInventoryItemSlot*> GetInventorySlots() const;
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Inventory Items")
		const TArray<class UInventoryItemSlot*> GetInventorySlotsOfSize(EItemSize Size) const;


};
