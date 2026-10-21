// Fill out your copyright notice in the Description page of Project Settings.


#include "Inventory/InventoryItem.h"
#include "Components/InventoryItemComponent.h"
#include "Components/InventoryManagerComponent.h"
#include "Inventory/Inventory.h"
#include "Components/SceneComponent.h"
#include "Components/InteractiveComponent.h"
#include "Components/InventoryAttachmentComponent.h"
#include "Components/PrimitiveComponent.h"
#include "Components/PlayerInventoryManagerComponent.h"
#include "Characters/FirstPersonCharacter.h"

/*actions*/
#include "Interactives/Actions/PickupAction.h"
#include "Interactives/Actions/DropAction.h"


// Sets default values

AInventoryItem::AInventoryItem(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	SetRootComponent(ObjectInitializer.CreateDefaultSubobject<USceneComponent>(this, TEXT("ItemRoot")));
	InteractiveComponent = ObjectInitializer.CreateDefaultSubobject<UInteractiveComponent>(this, TEXT("Interactive Component"));
	if (InteractiveComponent)
	{
		InteractiveComponent->SetupAttachment(GetRootComponent());
		InteractiveComponent->ActionConfigs.Add(FInteractiveActionConfig(UPickupAction::StaticClass()));
		InteractiveComponent->ActionConfigs.Add(FInteractiveActionConfig(UDropAction::StaticClass()));
	}


	ItemComponent = ObjectInitializer.CreateDefaultSubobject<UInventoryItemComponent>(this, TEXT("ItemComp"));
	if (ItemComponent)
	{
		AddOwnedComponent(ItemComponent);

		ItemComponent->bAutoEquip = false;
	}



	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;
    bReplicates = true;
    SetReplicateMovement(true);
}

// Called when the game starts or when spawned
void AInventoryItem::BeginPlay()
{
	Super::BeginPlay();	
}

// Called every frame
void AInventoryItem::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

//================================================
//==================INTERACTION===================
//================================================

void AInventoryItem::NativeOnInteractionStarted(AFirstPersonCharacter* User, UInteractiveComponent* Interactive, const UInteractiveAction* Action, const FInteractionContext& Context)
{
    if (IsValid(User) && Action && Action->IsA<UPickupAction>() && ItemComponent)
        StartPickup(User, ItemComponent, User->GetInventoryManager(), Context);
}

void AInventoryItem::NativeOnInteractionCompleted(AFirstPersonCharacter* User, UInteractiveComponent* Interactive, const UInteractiveAction* Action, const FInteractionContext& Context)
{
    if (!IsValid(User) || !IsValid(Action) || !User->HasAuthority() || !ItemComponent)
		return;

	UInventoryManagerComponent* Receiving = User->GetInventoryManager();

    if (!Receiving)
		return;


    if (Action->IsA<UPickupAction>())
    {
        const bool bSucceeded = Receiving->PickupItem(this, Context);

        StopPickup(User, ItemComponent, Receiving, Context, bSucceeded ? EInventoryItemOperationEndReason::Completed : EInventoryItemOperationEndReason::Interrupted);
    }
    else if (Action->IsA<UDropAction>())
    {
        if (UInventoryManagerComponent* Holder = ItemComponent->GetInventoryManager())
            Holder->DropItem(this);
    }
}

void AInventoryItem::NativeOnInteractionCanceled(AFirstPersonCharacter* User, UInteractiveComponent* Interactive, const UInteractiveAction* Action, const FInteractionContext& Context)
{
    if (Action && Action->IsA<UPickupAction>() && ItemComponent)
        StopPickup(User, ItemComponent, IsValid(User) ? User->GetInventoryManager() : nullptr, Context, Context.bInterrupted ? EInventoryItemOperationEndReason::Interrupted : EInventoryItemOperationEndReason::Canceled);
}

bool AInventoryItem::IsActionAvailable_Implementation(const AFirstPersonCharacter* User, const UInteractiveComponent* Interactive, const UInteractiveAction* Action) const
{
	if (!Action || !ItemComponent)
		return false;

	const bool bInWorld = ItemComponent->GetItemPresence() == EItemPresence::World;

	if (Action->IsA<UPickupAction>())
	{
		return bInWorld && IInventoryItemInterface::Execute_CanBePickedUp(this);
	}
	if (Action->IsA<UDropAction>())
	{
		return !bInWorld;
	}

	return false;
}

FText AInventoryItem::GetItemName()
{
	return GetItemComponent()->GetItemName();
}

UMaterialInterface* AInventoryItem::GetItemIcon()
{
	return ItemComponent->GetItemIcon();
}

EItemSize AInventoryItem::GetItemSize()
{
	if (ItemComponent)
		return ItemComponent->GetItemSize();
	else
		return EItemSize::Small;
}

EItemPresence AInventoryItem::GetItemPresence()
{
	return ItemComponent->GetItemPresence();
}

bool AInventoryItem::CanBePickedUp_Implementation() const
{
	if (!ItemComponent || ItemComponent->GetItemPresence() != EItemPresence::World)
		return false;

	return true;
}

void AInventoryItem::NativeOnItemStored(AActor* StorageActor)
{
    if (!IsValid(StorageActor) || !ItemComponent)
        return;

    TArray<UPrimitiveComponent*> Primitives;

    GetComponents(Primitives);
    for (UPrimitiveComponent* Primitive : Primitives)
        Primitive->SetSimulatePhysics(false);
    AttachToActor(StorageActor, FAttachmentTransformRules::KeepWorldTransform);
    SetActorHiddenInGame(true);
}

void AInventoryItem::NativeOnEquippingCompleted(AFirstPersonCharacter* User, UInventoryItemComponent* Component, UInventoryManagerComponent* Manager)
{
    TArray<UPrimitiveComponent*> Primitives;

    GetComponents(Primitives);
    for (UPrimitiveComponent* Primitive : Primitives) Primitive->SetSimulatePhysics(false);
    if (Component && Component->GetAttachmentSlot()) Component->GetAttachmentSlot()->ApplyAttachment();
    SetActorHiddenInGame(false);
}

void AInventoryItem::NativeOnItemPickedUp(AActor* ReceivingActor, const FInteractionContext& Context)
{

}

void AInventoryItem::NativeOnItemDropped(AActor* RejectingActor)
{
    DetachFromActor(FDetachmentTransformRules::KeepWorldTransform);
    SetActorHiddenInGame(false);
    SetActorEnableCollision(true);
}

UInventoryItemComponent* AInventoryItem::GetItemComponent()
{
	return ItemComponent;
}

//================================================
//==================ATTACHMENTS===================
//================================================

void AInventoryItem::OnItemAttached(AInventoryItem* Item, UInventoryAttachmentComponent* AttachedComponent)
{

}

//================================================
//====================MANAGER=====================
//================================================

UInventoryManagerComponent* AInventoryItem::GetInventoryManager()
{
    return FindComponentByClass<UInventoryManagerComponent>();
}

//================================================
//======================GRID======================
//================================================

void AInventoryItem::UpdateGridKey(FIntPoint NewGridKey)
{
	RegisteredGridKey = NewGridKey;
}

