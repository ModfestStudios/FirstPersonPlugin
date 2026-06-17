// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/Components/Entries/InventoryEquipmentSlotEntry.h"
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

bool UInventoryEquipmentSlotEntry::IsItemEquipped()
{
    if (!EquipmentSlot)
        return false;
    else
        return EquipmentSlot->IsItemEquipped();
}

UInventoryEquipmentSlot* UInventoryEquipmentSlotEntry::GetEquipmentSlot()
{
    return EquipmentSlot;
}

void UInventoryEquipmentSlotEntry::SetEquipmentSlot(UInventoryEquipmentSlot* NewEquipmentSlot)
{
    EquipmentSlot = NewEquipmentSlot;

}

void UInventoryEquipmentSlotEntry::NativeOnInitialized()
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

FReply UInventoryEquipmentSlotEntry::NativeOnMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
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

void UInventoryEquipmentSlotEntry::NativeOnDragEnter(const FGeometry& InGeometry, const FDragDropEvent& InDragDropEvent, UDragDropOperation* InOperation)
{
}

void UInventoryEquipmentSlotEntry::NativeOnDragLeave(const FDragDropEvent& InDragDropEvent, UDragDropOperation* InOperation)
{
}

void UInventoryEquipmentSlotEntry::NativeOnListItemObjectSet(UObject* ListItemObject)
{
    if (ListItemObject->IsA<UInventoryEquipmentSlot>())
        SetEquipmentSlot(Cast<UInventoryEquipmentSlot>(ListItemObject));

    Execute_OnListItemObjectSet(Cast<UObject>(this), ListItemObject);
}

void UInventoryEquipmentSlotEntry::NativeOnItemSelectionChanged(bool bIsSelected)
{
    Execute_BP_OnItemSelectionChanged(Cast<UObject>(this), bIsSelected);

}
