// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/Components/Entries/InventorySlotEntry.h"
#include "Inventory/InventoryEquipmentSlot.h"

#include "Characters/FirstPersonCharacter.h"

/*inventory*/
#include "Components/InventoryManagerComponent.h"
#include "Components/PlayerInventoryManagerComponent.h"

/*utilities*/
#include "Blueprint/DragDropOperation.h"
#include "Blueprint/WidgetBlueprintLibrary.h"
#include "InputCoreTypes.h"
#include "GameFramework/InputSettings.h"




bool UInventorySlotEntry::IsItemEquipped()
{
    if (!EquipmentSlot)
        return false;
    else
        return EquipmentSlot->IsItemEquipped();
}

void UInventorySlotEntry::SetEquipmentSlot(UInventoryEquipmentSlot* NewEquipmentSlot)
{
    EquipmentSlot = NewEquipmentSlot;
}

void UInventorySlotEntry::NativeOnInitialized()
{
    /*auto-*/
    if (bAutoInitializeSlot && !EquipmentSlotID.IsNone())
    {
        /*if a first person character*/
        if (AFirstPersonCharacter* Char = GetOwningPlayerPawn<AFirstPersonCharacter>())
        {
            EquipmentSlot = Char->GetInventoryManager()->GetEquipmentSlot(EquipmentSlotID);
        }
    }

    Super::NativeOnInitialized();
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
    if (ListItemObject->IsA<UInventoryEquipmentSlot>())
        SetEquipmentSlot(Cast<UInventoryEquipmentSlot>(ListItemObject));

    Execute_OnListItemObjectSet(Cast<UObject>(this), ListItemObject);
}

void UInventorySlotEntry::NativeOnItemSelectionChanged(bool bIsSelected)
{

    Execute_BP_OnItemSelectionChanged(Cast<UObject>(this), bIsSelected);
}
