// Fill out your copyright notice in the Description page of Project Settings.

/*InventorySlotWidget - the base class for creating drag and drop inventory slots/items*/

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Blueprint/IUserObjectListEntry.h"
#include "Components/InventoryManagerComponent.h"
#include "InventorySlotEntry.generated.h"


/// <summary>
/// @TODO: I believe I just need to delete htis. THis was converted to "InventoryEquipmentSlot" to be consistent with naming conventions
/// </summary>

struct FInventoryEquipmentSlot;

/**
 * 
 */
UCLASS(abstract, meta = (DisplayName = "InventoryEquipmentSlotEntry"))
class FIRSTPERSONMODULE_API UInventorySlotEntry : public UUserWidget, public IUserObjectListEntry
{
	GENERATED_BODY()
public:

	///*the item we represent*/
	//UPROPERTY(BlueprintReadOnly, Category = "Inventory Item")
	//	class UInventoryItemComponent* InventoryItem;
	//UPROPERTY(BlueprintReadOnly, Category = "Inventory Item")
	//	class AActor* ItemActor;

	/*if enabled - will automatically grab the matching Equipment Slot from the owning Inventory Manager*/
	UPROPERTY(EditAnywhere, Category = "Equipment Slot")
		bool bAutoInitializeSlot = true;
	UPROPERTY(EditAnywhere, Category = "Equipment Slot")
		FName EquipmentSlotID;
	UPROPERTY()
		class UInventoryEquipmentSlot* EquipmentSlot;

	//=======================================================================================================================================================
	//=======================================================================FUNCTIONS=======================================================================
	//=======================================================================================================================================================
	
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Equipment Slot")
		virtual bool IsItemEquipped();

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
