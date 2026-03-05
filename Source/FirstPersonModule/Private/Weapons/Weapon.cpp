// Copyright (c) 2022 Pocket Sized Animations


#include "Weapons/Weapon.h"

/*components*/
#include "Components/SceneComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "Components/StaticMeshComponent.h"


/*first person*/
#include "Components/FirstPersonViewComponent.h"
#include "Components/FirstPersonItemComponent.h"
#include "Characters/FirstPersonCharacter.h"


/*inventory*/
#include "Components/InventoryManagerComponent.h"
#include "Components/InventoryItemComponent.h"

/*interaction*/
#include "Components/InteractiveCollisionComponent.h"
#include "GameFramework/Character.h"


// Sets default values
AWeapon::AWeapon(const FObjectInitializer& ObjectInitializer)
	:Super(ObjectInitializer)
{
	/*generate world/3rd person mesh*/
	Mesh = ObjectInitializer.CreateDefaultSubobject<USkeletalMeshComponent>(this, TEXT("MeshComp"));
	if (Mesh)
	{
		SetRootComponent(Mesh);

		Mesh->bOwnerNoSee = true;
	}

	/*generate first person mesh*/
	FirstPersonComponent = ObjectInitializer.CreateDefaultSubobject<UFirstPersonItemComponent>(this, TEXT("FirstPersonComponent"));


	ItemComponent = ObjectInitializer.CreateDefaultSubobject<UInventoryItemComponent>(this, TEXT("ItemComp"));
	if (ItemComponent)
	{
		AddOwnedComponent(ItemComponent);

		ItemComponent->bAutoEquip = true;
	}

	InteractionCollision = ObjectInitializer.CreateDefaultSubobject<UInteractiveCollisionComponent>(this, TEXT("InteractiveCollisionComponent"));
	if (InteractionCollision)
	{
		InteractionCollision->SetupAttachment(GetRootComponent());
		//InteractionCollision->SetBoxExtent(FVector())
	}

	bReplicates = true;
	NetPriority = 3.0f; //similar to PlayerController
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void AWeapon::BeginPlay()
{
	Super::BeginPlay();

}

// Called every frame
void AWeapon::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AWeapon::BeginFire()
{

}

void AWeapon::EndFire()
{

}

/*OnEquip() - Called by the ItemComponent whenever the InventoryManager attempts to equip it - allowing Weapon specific code to be called
* @note - the ItemComponent puts itself into limbo and expects its Actor (the Weapon) to call the ItemComponent->EndEquip() in order to finalize
* its states for replication and syncing
*
*/
void AWeapon::OnEquip_Implementation()
{
	/*@todo: eventually we'll want a transitional period between grabbing the weapon  immedietely and having an animation play where the hands are reaching for the gun
	prior to it actually attaching to the socket*/


	AttachThirdPersonMesh();
	AttachFirstPersonMesh();

	/*disable collision*/
	DisableCollision(); //prevents players getting stuck on an item they're holding
	SetActorHiddenInGame(false); //show model again

	/*notify the ItemComponent we've finished equipping so it updates states*/
	if (ItemComponent)
		ItemComponent->EndEquip();
}

void AWeapon::OnUnequip_Implementation()
{

}

void AWeapon::OnPickedUp_Implementation()
{

}

//===================================
//============ATTACHMENTS============
//===================================


void AWeapon::AttachFirstPersonMesh()
{
	/*equip first person mesh*/
	if (FirstPersonComponent && ItemComponent)
		FirstPersonComponent->AttachFirstPersonMesh(ItemComponent->GetOwningActor());
}


void AWeapon::AttachThirdPersonMesh()
{
	/*equip core actor to third person*/
	if (AFirstPersonCharacter* OwningChar = Cast<AFirstPersonCharacter>(GetOwner()))
		AttachToComponent(OwningChar->GetMesh(), FAttachmentTransformRules::SnapToTargetNotIncludingScale, OwningChar->RightHandSocketName);
}


//==================================
//============ANIMATIONS============
//==================================

void AWeapon::OnAnimNotify_AttachFirstPersonMesh()
{
	AttachFirstPersonMesh();
}

void AWeapon::OnAnimNotify_AttachThirdPersonMesh()
{
}


//===============================
//==========INTERACTION==========
//===============================

//void AWeapon::OnInteraction_Implementation(AFirstPersonCharacter* User)
//{
//	/*notify inventory manager to pick us up*/
//	if (User)
//	{
//		if (UInventoryManagerComponent* InvManager = User->GetInventoryManager())
//		{
//			InvManager->PickupItem(this);
//		}
//	}
//}
//
//bool AWeapon::IsInteractionAvailable_Implementation(AFirstPersonCharacter* User)
//{
//	return true;
//}

void AWeapon::DisableCollision()
{
	SetActorEnableCollision(false);
}

bool AWeapon::IsLocallyControlled()
{
	if (APawn* OwningPawn = GetOwningPawn())
		return OwningPawn->IsLocallyControlled();
	else
		return false;
}

APawn* AWeapon::GetOwningPawn()
{
	if (Owner && (Owner->IsA(APawn::StaticClass()) || Owner->GetClass()->IsChildOf<APawn>()))
		return Cast<APawn>(Owner);

	return nullptr;
}

APlayerController* AWeapon::GetOwningPlayerController()
{
	if (GetOwningPawn())
		return GetOwningPawn()->GetController<APlayerController>();
	else
		return nullptr;
}
