// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/Components/Entries/InventoryAttachmentCompEntry.h"
#include "Components/InventoryAttachmentComponent.h"
#include "Characters/FirstPersonCharacter.h"
#include "Inventory/InventoryItem.h"

void UInventoryAttachmentCompEntry::NativeOnInitialized()
{
    /*auto-*/
    if (bAutoInitialize)
    {
        if (AttachmentID.IsNone()  == false)
        {
            if (AFirstPersonCharacter* Char = GetOwningPlayerPawn<AFirstPersonCharacter>())            
                AttachmentComponent = Char->GetEquipmentSlotByID(AttachmentID);                     
        }
        else if (ItemType != nullptr)
        {
            if (AFirstPersonCharacter* Char = GetOwningPlayerPawn<AFirstPersonCharacter>())
                AttachmentComponent = Char->GetEquipmentSlot(ItemType);
        }
    }

    //if (bAutoInitializeSlot && !EquipmentSlotID.IsNone())
    //{
    //    /*if a first person character*/
    //    if (AFirstPersonCharacter* Char = GetOwningPlayerPawn<AFirstPersonCharacter>())
    //    {
    //        EquipmentSlot = Char->GetInventoryManager()->GetEquipmentSlot(EquipmentSlotID);
    //    }
    //}
	
	Super::NativeOnInitialized();
}

void UInventoryAttachmentCompEntry::NativeOnListItemObjectSet(UObject* ListItemObject)
{
    
    if (ListItemObject->IsA<UInventoryAttachmentComponent>())
        SetAttachmentComponent(Cast<UInventoryAttachmentComponent>(ListItemObject));

    Execute_OnListItemObjectSet(Cast<UObject>(this), ListItemObject);
}

void UInventoryAttachmentCompEntry::SetAttachmentComponent(UInventoryAttachmentComponent* NewAttachment)
{
    if (!AttachmentComponent)
        return;

    AttachmentComponent = NewAttachment;

    /*notify*/
    OnAttachmentComponentChanged();
}

void UInventoryAttachmentCompEntry::OnAttachmentComponentChanged()
{
    BP_OnAttachmentComponentChanged();
}

