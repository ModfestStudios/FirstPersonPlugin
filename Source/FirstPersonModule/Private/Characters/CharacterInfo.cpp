// Fill out your copyright notice in the Description page of Project Settings.


#include "Characters/CharacterInfo.h"
#include "Characters/CharacterAsset.h"
#include "Rosters/RosterInfo.h"
#include "Subsystems/CharacterSubsystem.h"
#include "Inventory/InventoryLoadout.h"

/*engine*/
#include "Engine/World.h"
#include "Engine/GameInstance.h"

/*network*/
#include "Net/UnrealNetwork.h"


void ACharacterInfo::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ACharacterInfo, FirstName);
	DOREPLIFETIME(ACharacterInfo, LastName);
	DOREPLIFETIME(ACharacterInfo, Callsign);

}

ACharacterInfo::ACharacterInfo()
{	
	bReplicates = true;
	bAlwaysRelevant = true;
}


void ACharacterInfo::BeginPlay()
{	
	/*avoid doing this multiple times as BeginPlay has potential of running more than once*/
	if (GetNetMode() < NM_Client && !HasActorBegunPlay())
	{
		RandomizeName();
		Loadout = GetWorld()->SpawnActor<AInventoryLoadout>();
		Loadout->SetOwner(this); //default to this - but should should be overwritten when assigned to a player
	}	

	Super::BeginPlay();
}

void ACharacterInfo::InitFromAsset(UCharacterAsset* Asset)
{
	if (!Asset)
		return;

	FirstName = Asset->FirstName;
	
	LastName = Asset->LastName;

//	PrimaryWeapon = Asset->PrimaryWeapon;
}

void ACharacterInfo::RandomizeName()
{
	if (GetGameInstance() == nullptr)
		return;

	if (UCharacterSubsystem* CSS = GetGameInstance()->GetSubsystem<UCharacterSubsystem>())
	{
		if (FirstName.IsEmpty())
			FirstName = CSS->GetRandomFirstName(Gender);
		if (LastName.IsEmpty())
			LastName = CSS->GetRandomLastName();
		if(Callsign.IsEmpty())
			Callsign = CSS->GetRandomCallsign();
	}
}

FString ACharacterInfo::GetLastNameFirstName()
{
	return LastName + ", " + FirstName;
}

bool ACharacterInfo::IsSelected()
{
	return false;
}

bool ACharacterInfo::IsAssignedToRoster()
{
	return AssignedRoster != nullptr;
}

void ACharacterInfo::SetAssignedRoster(ARosterInfo* Roster)
{
	if (AssignedRoster != nullptr && AssignedRoster != Roster)
		AssignedRoster->RemoveTeamMember(this);

	AssignedRoster = Roster;
}

void ACharacterInfo::ClearAssignedRoster()
{
	AssignedRoster = nullptr;
}
