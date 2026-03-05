// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/Components/Entries/InventorySlotEntry.h"
#include "Components/InventoryItemComponent.h"

/*utilities*/
#include "Blueprint/DragDropOperation.h"
#include "Blueprint/WidgetBlueprintLibrary.h"
#include "InputCoreTypes.h"
#include "GameFramework/InputSettings.h"

void UInventorySlotEntry::InitializeInventorySlot(UInventoryItemComponent* ItemComponent)
{
    if (!ItemComponent)
        return;

    InventoryItem = ItemComponent;
    ItemActor = ItemComponent->GetOwner();
}

FText UInventorySlotEntry::GetItemName()
{
    if (InventoryItem)
        return InventoryItem->ItemName;

    return FText();
}

FReply UInventorySlotEntry::NativeOnMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
    if (InMouseEvent.IsMouseButtonDown(EKeys::LeftMouseButton))
    {
        UDragDropOperation* DragDropOp = NewObject<UDragDropOperation>();
        DragDropOp->DefaultDragVisual = this; // Set this widget as the visual.
        DragDropOp->Payload = this; // Attach any data you need (e.g., item reference).

        return UWidgetBlueprintLibrary::DetectDragIfPressed(InMouseEvent, this, EKeys::LeftMouseButton).NativeReply;
    }

    return FReply::Unhandled();
}

void UInventorySlotEntry::NativeOnDragEnter(const FGeometry& InGeometry, const FDragDropEvent& InDragDropEvent, UDragDropOperation* InOperation)
{
}

void UInventorySlotEntry::NativeOnDragLeave(const FDragDropEvent& InDragDropEvent, UDragDropOperation* InOperation)
{
}

void UInventorySlotEntry::NativeOnListItemObjectSet(UObject* ListItemObject)
{
    InitializeInventorySlot(Cast<UInventoryItemComponent>(ListItemObject));

    Execute_OnListItemObjectSet(Cast<UObject>(this), ListItemObject);
}

void UInventorySlotEntry::NativeOnItemSelectionChanged(bool bIsSelected)
{

    Execute_BP_OnItemSelectionChanged(Cast<UObject>(this), bIsSelected);
}
