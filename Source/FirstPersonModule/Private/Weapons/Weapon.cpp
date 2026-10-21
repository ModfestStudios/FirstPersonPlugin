// Copyright (c) 2022 Pocket Sized Animations


#include "Weapons/Weapon.h"

/*components*/
#include "Components/SceneComponent.h"
#include "Components/SkeletalMeshComponent.h"


/*first person*/
#include "Components/FirstPersonItemComponent.h"
#include "Characters/FirstPersonCharacter.h"


/*inventory*/
#include "Components/InventoryItemComponent.h"

/*interaction*/
#include "Components/InteractiveComponent.h"
#include "GameFramework/Character.h"


// Sets default values

AWeapon::AWeapon(const FObjectInitializer& ObjectInitializer)
	:Super(ObjectInitializer)
{
	/*generate world/3rd person mesh*/
	Mesh = ObjectInitializer.CreateDefaultSubobject<USkeletalMeshComponent>(this, TEXT("MeshComp"));
	if (Mesh)
	{
		USceneComponent* ItemRoot = GetRootComponent();

		SetRootComponent(Mesh);
		if (ItemRoot)
			ItemRoot->SetupAttachment(Mesh);

		Mesh->bOwnerNoSee = true;
	}

	/*generate first person mesh*/
	FirstPersonComponent = ObjectInitializer.CreateDefaultSubobject<UFirstPersonItemComponent>(this, TEXT("FirstPersonComponent"));

	if (ItemComponent)
	{
		ItemComponent->bAutoEquip = true;
	}

	if (InteractiveComponent)
		InteractiveComponent->SetupAttachment(GetRootComponent());

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

//================================================
//==================ATTACHMENTS===================
//================================================

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

//================================================
//===================ANIMATIONS===================
//================================================

void AWeapon::OnAnimNotify_AttachFirstPersonMesh()
{
	AttachFirstPersonMesh();
}

void AWeapon::OnAnimNotify_AttachThirdPersonMesh()
{
}

//================================================
//===================COLLISION====================
//================================================

void AWeapon::DisableCollision()
{
	SetActorEnableCollision(false);
}

//================================================
//===================OWNERSHIP====================
//================================================

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
