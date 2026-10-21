// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/Components/Entries/InventoryAttachmentCompEntry.h"
#include "Components/InventoryAttachmentComponent.h"
#include "Components/InventoryManagerComponent.h"
#include "GameFramework/Pawn.h"
#include "Inventory/InventoryItem.h"

void UInventoryAttachmentCompEntry::NativeOnInitialized()
{
    RefreshAttachmentComponent();
    Super::NativeOnInitialized();
}

void UInventoryAttachmentCompEntry::NativeConstruct()
{
    if (bAutoInitialize && !bUsesListItemBinding && !IsValid(AttachmentComponent))
        RefreshAttachmentComponent();

    Super::NativeConstruct();
}

void UInventoryAttachmentCompEntry::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
    // Stop searching once bound; list entries are assigned by their list instead.
    if (bAutoInitialize && !bUsesListItemBinding && !IsValid(AttachmentComponent))
        RefreshAttachmentComponent();

    Super::NativeTick(MyGeometry, InDeltaTime);
}

void UInventoryAttachmentCompEntry::NativeOnListItemObjectSet(UObject* ListItemObject)
{
    UInventoryAttachmentComponent* NewAttachment = Cast<UInventoryAttachmentComponent>(ListItemObject);

    bUsesListItemBinding = true;
    SetAttachmentComponent(IsValid(NewAttachment) ? NewAttachment : nullptr);
    Execute_OnListItemObjectSet(this, ListItemObject);
}

bool UInventoryAttachmentCompEntry::RefreshAttachmentComponent()
{
    if (!bAutoInitialize || bUsesListItemBinding)
        return false;

    APawn* Pawn = GetOwningPlayerPawn();
    UInventoryManagerComponent* Inventory = IsValid(Pawn) ? Pawn->FindComponentByClass<UInventoryManagerComponent>() : nullptr;
    UInventoryAttachmentComponent* MatchingAttachment = nullptr;

    if (!IsValid(Inventory))
        return false;

    if (!AttachmentID.IsNone())
        MatchingAttachment = Inventory->GetEquipmentSlot(AttachmentID);
    else if (ItemType)
        MatchingAttachment = Inventory->GetEquipmentSlotByClass(ItemType);

    if (!IsValid(MatchingAttachment))
        return false;

    SetAttachmentComponent(MatchingAttachment);
    return true;
}

void UInventoryAttachmentCompEntry::SetAttachmentComponent(UInventoryAttachmentComponent* NewAttachment)
{
    if (AttachmentComponent == NewAttachment)
        return;

    AttachmentComponent = NewAttachment;
    OnAttachmentComponentChanged();
}

bool UInventoryAttachmentCompEntry::HasValidAttachedItem() const
{
    if (!IsValid(AttachmentComponent))
        return false;

    return AttachmentComponent->IsItemEquipped();
}

void UInventoryAttachmentCompEntry::OnAttachmentComponentChanged()
{
    BP_OnAttachmentComponentChanged();
}
