// Fill out your copyright notice in the Description page of Project Settings.

/*InventorySlotWidget - the base class for creating drag and drop inventory slots/items*/

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Blueprint/IUserObjectListEntry.h"
#include "InventorySlotEntry.generated.h"

/**
 * 
 */
UCLASS(abstract)
class FIRSTPERSONMODULE_API UInventorySlotEntry : public UUserWidget, public IUserObjectListEntry
{
	GENERATED_BODY()
public:

	/*the item we represent*/
	UPROPERTY(BlueprintReadOnly, Category = "Inventory Item")
		class UInventoryItemComponent* InventoryItem;
	UPROPERTY(BlueprintReadOnly, Category = "Inventory Item")
		class AActor* ItemActor;



	//=======================================================================================================================================================
	//=======================================================================FUNCTIONS=======================================================================
	//=======================================================================================================================================================
	UFUNCTION()
		virtual void InitializeInventorySlot(class UInventoryItemComponent* ItemComponent);

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Inventory Item")
		FText GetItemName();

	/*drag and drop functionality*/
	virtual FReply NativeOnMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent) override;
	virtual void NativeOnDragEnter(const FGeometry& InGeometry, const FDragDropEvent& InDragDropEvent, UDragDropOperation* InOperation) override;
	virtual void NativeOnDragLeave(const FDragDropEvent& InDragDropEvent, UDragDropOperation* InOperation) override;

	/*object is passed in - should be converted to UMapInfo and setse MapInfo*/
	virtual void NativeOnListItemObjectSet(UObject* ListItemObject) override;
	/*selection state*/
	virtual void NativeOnItemSelectionChanged(bool bIsSelected) override;
	


	
};
