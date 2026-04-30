// Fill out your copyright notice in the Description page of Project Settings.


#include "Components/InventoryAttachmentComponent.h"
#include "Components/InventoryManagerComponent.h"
#include "Components/InventoryItemComponent.h"

/*characters*/
#include "Characters/FirstPersonCharacter.h"

/*inventory*/
#include "Inventory/Inventory.h"
#include "Inventory/InventoryItem.h"


// Sets default values for this component's properties
UInventoryAttachmentComponent::UInventoryAttachmentComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;
	SetIsReplicatedByDefault(true);

	// ...
}


// Called when the game starts
void UInventoryAttachmentComponent::BeginPlay()
{
	Super::BeginPlay();
		
	if (DefaultAttachment != nullptr && GetNetMode() < NM_Client)
		SpawnDefaultAttachment();

}


// Called every frame
void UInventoryAttachmentComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}

AInventoryItem* UInventoryAttachmentComponent::GetAttachedItem()
{
	return AttachedItem;
}

UInventoryManagerComponent* UInventoryAttachmentComponent::GetAttachedInventoryManager()
{
	if (GetAttachedItem())
	{
		return GetAttachedItem()->GetInventoryManager();
	}

	return nullptr;
}

void UInventoryAttachmentComponent::SpawnDefaultAttachment()
{
	/*safety/server check*/
	if (GetNetMode() == NM_Client || !GetOwner())
		return;

	/*initialize*/
	TSubclassOf<AInventoryItem> ItemClass = DefaultAttachment;
	FVector SpawnLocation = GetOwner()->GetActorLocation();
	FRotator SpawnRotation = GetOwner()->GetActorRotation();
	FActorSpawnParameters SpawnParams;
	SpawnParams.Owner = GetOwner();
	SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

	/*spawn the attachment*/
	AttachedItem = GetWorld()->SpawnActor<AInventoryItem>(ItemClass,SpawnLocation,SpawnRotation,SpawnParams);

	if (AttachedItem)
	{
		AttachedItem->SetActorEnableCollision(false);
		AttachedItem->SetActorHiddenInGame(true); //hide it by default - it's up to the actor to unhide it
	}

	/*notify owner of attachment*/
	if (AInventoryItem* OwnerAsItem = GetOwner<AInventoryItem>())
	{
		OwnerAsItem->OnItemAttached(AttachedItem, this);
	}

	if (AFirstPersonCharacter* OwnerAsCharacter = GetOwner<AFirstPersonCharacter>())
	{
		OwnerAsCharacter->OnItemEquipped(AttachedItem, this);
	}
}

