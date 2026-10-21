// Copyright (c) Modfest Studios
#include "Components/InventoryAttachmentComponent.h"
#include "Components/InventoryManagerComponent.h"
#include "Inventory/InventoryItem.h"
#include "Components/SceneComponent.h"
#include "Net/UnrealNetwork.h"

UInventoryAttachmentComponent::UInventoryAttachmentComponent()
{
    PrimaryComponentTick.bCanEverTick = false;
    SetIsReplicatedByDefault(true);
}

void UInventoryAttachmentComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
    Super::GetLifetimeReplicatedProps(OutLifetimeProps);
    DOREPLIFETIME(UInventoryAttachmentComponent, Config);
    DOREPLIFETIME(UInventoryAttachmentComponent, InventoryManager);
    DOREPLIFETIME(UInventoryAttachmentComponent, AttachedItem);
}

AInventoryItem* UInventoryAttachmentComponent::GetAttachedItem() const { return AttachedItem; }

UInventoryManagerComponent* UInventoryAttachmentComponent::GetAttachedInventoryManager() const
{
    return IsValid(AttachedItem) ? AttachedItem->FindComponentByClass<UInventoryManagerComponent>() : nullptr;
}

bool UInventoryAttachmentComponent::IsItemEquipped() const { return IsValid(AttachedItem); }

bool UInventoryAttachmentComponent::CanAcceptItem(const AInventoryItem* Item) const
{
    if (!IsValid(Item) || IsItemEquipped())
        return false;
    // Specialized slots require a type; hands may accept any inventory item.
    if (!Config.AcceptedItemClass)
        return Config.SlotKind != EInventoryEquipmentSlotKind::Equipment;
    return Item->IsA(Config.AcceptedItemClass);
}

void UInventoryAttachmentComponent::ApplyAttachment()
{
    if (IsValid(AttachedItem))
        if (USceneComponent* Target = GetAttachmentTarget())
        {
            AttachedItem->AttachToComponent(Target, FAttachmentTransformRules::SnapToTargetNotIncludingScale, Config.AttachmentSocket);
            AttachedItem->SetActorRelativeTransform(Config.AttachmentTransform);
        }
}

USceneComponent* UInventoryAttachmentComponent::GetAttachmentTarget() const
{
    if (!GetOwner())
        return nullptr;

    TArray<USceneComponent*> Components;

    GetOwner()->GetComponents(Components);
    for (USceneComponent* Component : Components)
        if ((!Config.AttachmentComponentName.IsNone() && Component->GetFName() == Config.AttachmentComponentName) ||
            (Config.AttachmentComponentName.IsNone() && !Config.AttachmentSocket.IsNone() && Component->DoesSocketExist(Config.AttachmentSocket)))
            return Component;
    return GetOwner()->GetRootComponent();
}

void UInventoryAttachmentComponent::OnRep_AttachedItem() { ApplyAttachment(); }

