// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/Components/Entries/InventoryItemSlotEntry.h"
#include "Inventory/InventoryItemSlot.h"


/*utilities*/
#include "Blueprint/DragDropOperation.h"
#include "Blueprint/WidgetBlueprintLibrary.h"
#include "InputCoreTypes.h"
#include "GameFramework/InputSettings.h"

FText UInventoryItemSlotEntry::GetItemName() const
{
    if (InventoryItemSlot)
        return InventoryItemSlot->GetItemName();
    else
        return FText();
}

UMaterialInterface* UInventoryItemSlotEntry::GetItemIcon() const
{
    if (InventoryItemSlot)
        return InventoryItemSlot->GetItemIcon();
    else
        return nullptr;
}

bool UInventoryItemSlotEntry::IsItemEquipped()
{
	return false;
}

void UInventoryItemSlotEntry::SetInventorySlot(UInventoryItemSlot* ItemSlot)
{
    InventoryItemSlot = ItemSlot;
}


void UInventoryItemSlotEntry::NativeOnInitialized()
{
}

//FReply UInventoryItemSlotEntry::NativeOnMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
//{
//    return NativeOnMouseButtonDown(InGeometry, InMouseEvent);
//    //if (InMouseEvent.IsMouseButtonDown(EKeys::LeftMouseButton))
//    //{
//    //    UDragDropOperation* DragDropOp = NewObject<UDragDropOperation>();
//    //    DragDropOp->DefaultDragVisual = this; // Set this widget as the visual.
//    //    DragDropOp->Payload = this; // Attach any data you need (e.g., item reference).
//
//    //    return UWidgetBlueprintLibrary::DetectDragIfPressed(InMouseEvent, this, EKeys::LeftMouseButton).NativeReply;
//    //}
//
//    //return FReply::Unhandled();
//}
//
//void UInventoryItemSlotEntry::NativeOnDragEnter(const FGeometry& InGeometry, const FDragDropEvent& InDragDropEvent, UDragDropOperation* InOperation)
//{
//    Super::NativeOnDragEnter(InGeometry, InDragDropEvent, InOperation);
//}
//
//void UInventoryItemSlotEntry::NativeOnDragLeave(const FDragDropEvent& InDragDropEvent, UDragDropOperation* InOperation)
//{
//    Super::NativeOnDragLeave(InDragDropEvent, InOperation);
//}

void UInventoryItemSlotEntry::NativeOnListItemObjectSet(UObject* ListItemObject)
{
    if (ListItemObject->IsA<UInventoryItemSlot>())
        SetInventorySlot(Cast<UInventoryItemSlot>(ListItemObject));

    Execute_OnListItemObjectSet(Cast<UObject>(this), ListItemObject);
}

void UInventoryItemSlotEntry::NativeOnItemSelectionChanged(bool bIsSelected)
{
}
