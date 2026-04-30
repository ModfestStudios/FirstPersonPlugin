// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Blueprint/IUserObjectListEntry.h"
#include "InventoryItemSlotEntry.generated.h"

/**
 * 
 */
UCLASS()
class FIRSTPERSONMODULE_API UInventoryItemSlotEntry : public UUserWidget, public IUserObjectListEntry
{
	GENERATED_BODY()
public:


	UPROPERTY(BlueprintReadOnly, Category = "Inventory Slot")
		class UInventoryItemSlot* InventoryItemSlot;

	//===============================================================================================================================================
	//===================================================================FUNCTIONS===================================================================
	//===============================================================================================================================================


	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Inventory Slot")
		FText GetItemName() const;
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Inventory Slot")
		class UMaterialInterface* GetItemIcon() const;



	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Equipment Slot")
		virtual bool IsItemEquipped();

	UFUNCTION(BlueprintCallable, Category = "Equipment Slot")
		virtual void SetInventorySlot(class UInventoryItemSlot* ItemSlot);



	virtual void NativeOnInitialized() override;

	/*drag and drop functionality*/
	//virtual FReply NativeOnMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent) override;
	//virtual void NativeOnDragEnter(const FGeometry& InGeometry, const FDragDropEvent& InDragDropEvent, UDragDropOperation* InOperation) override;
	//virtual void NativeOnDragLeave(const FDragDropEvent& InDragDropEvent, UDragDropOperation* InOperation) override;

	/*object is passed in - should be converted to UMapInfo and setse MapInfo*/
	virtual void NativeOnListItemObjectSet(UObject* ListItemObject) override;
	/*selection state*/
	virtual void NativeOnItemSelectionChanged(bool bIsSelected) override;
	
};
