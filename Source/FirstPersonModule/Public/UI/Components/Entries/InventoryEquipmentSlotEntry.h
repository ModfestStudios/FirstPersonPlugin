// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Blueprint/IUserObjectListEntry.h"
#include "InventoryEquipmentSlotEntry.generated.h"

/**
 * 
 */
UCLASS()
class FIRSTPERSONMODULE_API UInventoryEquipmentSlotEntry : public UUserWidget, public IUserObjectListEntry
{
	GENERATED_BODY()
public:


protected:
	/*if enabled - will automatically grab the matching Equipment Slot from the owning Inventory Manager*/
	UPROPERTY(EditAnywhere, Category = "Equipment Slot")
		bool bAutoInitializeSlot = true;
	UPROPERTY(EditAnywhere, Category = "Equipment Slot")
		FName EquipmentSlotID;
	UPROPERTY(BlueprintReadOnly, Category = "Equipment Slot")
		class UInventoryEquipmentSlot* EquipmentSlot;

	//===============================================================================================================================================
	//===================================================================FUNCTIONS===================================================================
	//===============================================================================================================================================

public:

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Equipment Slot")
		virtual bool IsItemEquipped();
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Equipment Slot")
		class UInventoryEquipmentSlot* GetEquipmentSlot();

	UFUNCTION(BlueprintCallable, Category = "Equipment Slot")
		virtual void SetEquipmentSlot(class UInventoryEquipmentSlot* NewEquipmentSlot);



	virtual void NativeOnInitialized() override;

	/*drag and drop functionality*/
	virtual FReply NativeOnMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent) override;
	virtual void NativeOnDragEnter(const FGeometry& InGeometry, const FDragDropEvent& InDragDropEvent, UDragDropOperation* InOperation) override;
	virtual void NativeOnDragLeave(const FDragDropEvent& InDragDropEvent, UDragDropOperation* InOperation) override;

	/*object is passed in - should be converted to UMapInfo and setse MapInfo*/
	virtual void NativeOnListItemObjectSet(UObject* ListItemObject) override;
	/*selection state*/
	virtual void NativeOnItemSelectionChanged(bool bIsSelected) override;

};
