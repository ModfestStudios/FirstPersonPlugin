// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/Components/Entries/InventoryEquipmentSlotEntry.h"
#include "Components/InventoryAttachmentComponent.h"

#include "Characters/FirstPersonCharacter.h"

/*inventory*/
#include "Components/InventoryManagerComponent.h"
#include "Components/PlayerInventoryManagerComponent.h"

/*utilities*/
#include "Blueprint/DragDropOperation.h"
#include "Blueprint/WidgetBlueprintLibrary.h"
#include "InputCoreTypes.h"
#include "GameFramework/InputSettings.h"

bool UInventoryAttachmentComponentEntry::IsItemEquipped()
{
    if (!EquipmentSlot)
        return false;
    else
        return EquipmentSlot->IsItemEquipped();
}

UInventoryAttachmentComponent* UInventoryAttachmentComponentEntry::GetEquipmentSlot()
{
    return EquipmentSlot;
}

void UInventoryAttachmentComponentEntry::SetEquipmentSlot(UInventoryAttachmentComponent* NewEquipmentSlot)
{
    EquipmentSlot = NewEquipmentSlot;

}

void UInventoryAttachmentComponentEntry::NativeOnInitialized()
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

FReply UInventoryAttachmentComponentEntry::NativeOnMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
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

void UInventoryAttachmentComponentEntry::NativeOnDragEnter(const FGeometry& InGeometry, const FDragDropEvent& InDragDropEvent, UDragDropOperation* InOperation)
{
}

void UInventoryAttachmentComponentEntry::NativeOnDragLeave(const FDragDropEvent& InDragDropEvent, UDragDropOperation* InOperation)
{
}

void UInventoryAttachmentComponentEntry::NativeOnListItemObjectSet(UObject* ListItemObject)
{
    if (ListItemObject->IsA<UInventoryAttachmentComponent>())
        SetEquipmentSlot(Cast<UInventoryAttachmentComponent>(ListItemObject));

    Execute_OnListItemObjectSet(Cast<UObject>(this), ListItemObject);
}

void UInventoryAttachmentComponentEntry::NativeOnItemSelectionChanged(bool bIsSelected)
{
    Execute_BP_OnItemSelectionChanged(Cast<UObject>(this), bIsSelected);

}
