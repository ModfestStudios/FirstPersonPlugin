// Copyright (c) 2022 Pocket Sized Animations


#include "Weapons/Firearm.h"

/*characters*/
#include "GameFramework/Character.h"
#include "Characters/FirstPersonCharacter.h"

/*first person*/
#include "Components/FirstPersonViewComponent.h"
#include "Components/FirstPersonItemComponent.h"

/*inventory*/
#include "Components/InventoryItemComponent.h"
#include "Components/InventoryItemComponent.h"

/*projetiles*/
#include "Subsystems/ProjectileSubsystem.h"
//#include "Components/ProjectileSpawnerComponent.h"

/*replication*/
#include "Net/UnrealNetwork.h"

/*setup replication*/
void AFirearm::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME_CONDITION(AFirearm, bFiring, COND_SkipOwner);
}

AFirearm::AFirearm(const FObjectInitializer& ObjectInitializer)
	:Super(ObjectInitializer)
{
	//ProjectileSpawnerComponent = ObjectInitializer.CreateDefaultSubobject<UProjectileSpawnerComponent>(this, TEXT("Projectile Spawner"));

	//if (ProjectileSpawnerComponent)
	//{
	//	ProjectileSpawnerComponent->SetupAttachment(Mesh);
	//}
}

/*BeginFire() - function that is called from the owner (usually the Player/Pawn) that initiates the weapon to begin the chain of events
to perform all the firing logic
*
*
*
*/

void AFirearm::BeginFire()
{
	Super::BeginFire();

	//if(GetNetMode() == NM_Client && IsLocallyControlled())

	if (CanFire())
		Fire();
	else
	{

	}

	//was originally using a Component to try and compartmentalize the code - but this did not feel appropriate
	/*if (ProjectileSpawnerComponent)
	{
		ProjectileSpawnerComponent->BeginFire();
	}*/
}

void AFirearm::EndFire()
{
	Super::EndFire();

	/*if (ProjectileSpawnerComponent)
	{
		ProjectileSpawnerComponent->EndFire();
	}*/
}

void AFirearm::Fire()
{
	bFiring = true;
	ShotsFired++; //increment the amount of shots fired

	SpawnProjectile(); //performs the shot
}

void AFirearm::SpawnProjectile()
{
	//UE_LOG(LogTemp, Log, TEXT("%s ProjectileType == Projectile"), NETMODE_WORLD);

	if (GetProjectileSubsystem())
	{
		/*get data on this projectiles location and direction*/
		FVector Location, Direction;
		GetProjectileSpawnLocation(Location, Direction);

		FProjectile Projectile = FProjectile(GetProjectileType(), Location, Direction, this, GetOwningPawn(), GetOwningPlayerController());
		GetProjectileSubsystem()->SpawnProjectile(Projectile);
	}
}

bool AFirearm::IsFiring()
{
	/*if (ProjectileSpawnerComponent)
		return ProjectileSpawnerComponent->IsFiring();
	*/
	/*fallback*/
	return false;
}

/*CanFire()
* Performs a few various checks to determine if this weapon can be fired
*
*
*/
bool AFirearm::CanFire()
{
	//@TODO: the actual thing
	return true;
}


void AFirearm::OnEquip_Implementation()
{
	Super::OnEquip_Implementation();

}

//===================================================
//====================PROJECTILES====================
//===================================================

UProjectileSubsystem* AFirearm::GetProjectileSubsystem()
{
	return ProjectileSubsystem ? ProjectileSubsystem : GetWorld()->GetSubsystem<UProjectileSubsystem>(); 
}

void AFirearm::GetProjectileSpawnLocation(FVector& Location, FVector& Direction)
{
	/*when using the PlayerView - if we're the server (authorative simulation) or the owning client we will use the PlayerController's view
	if we-re a remote client we will use an approximation start spot of the pawn's View Location (Actor Location + EyeHeight) and the Epic Replicated
	Base Aim Rotation (which is an approximation of the controller rotation sent to clients) */

	/*server || owning client*/
	if (GetNetMode() < NM_Client && GetOwningPlayerController() || (GetOwningPlayerController() && GetOwningPlayerController()->IsLocalPlayerController()))
	{
		FRotator _rotation;

		GetOwningPlayerController()->GetPlayerViewPoint(Location, _rotation);
		Direction = _rotation.Vector();

		return;
	}
	/*remote clients || if no PlayerController was found*/
	else if (APawn* P = GetOwningPawn())
	{
		Location = P->GetPawnViewLocation(); //Pawn's Location + EyeHeight offset
		Direction = P->GetBaseAimRotation().Vector(); //Epic replicated approxemiation of controller rotation

		return;
	}
}

UProjectileInfo* AFirearm::GetProjectileType()
{
	return ProjectileType;
}
