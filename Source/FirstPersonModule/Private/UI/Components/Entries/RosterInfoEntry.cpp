// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/Components/Entries/RosterInfoEntry.h"
#include "Rosters/RosterInfo.h"

/*engine*/
#include "Engine/GameInstance.h"

/*subsystems*/
#include "Subsystems/MissionSubsystem.h"

void URosterInfoEntry::NativeConstruct()
{
	Super::NativeConstruct();

	if (bAutoFindTeam && !TeamName.IsEmpty())
	{
		ARosterInfo* TeamRoster = GetRosterInfo(TeamName);
		SetRosterInfo(TeamRoster);
		RefreshRosterInfo();
	}
}

void URosterInfoEntry::NativeOnListItemObjectSet(UObject* ListItemObject)
{
	if (ARosterInfo* NewRoster = Cast<ARosterInfo>(ListItemObject))
		SetRosterInfo(NewRoster);

	Execute_OnListItemObjectSet(this, ListItemObject);
}

void URosterInfoEntry::SetRosterInfo(ARosterInfo* NewRoster)
{
	if (NewRoster == nullptr)
		return;

	/*cleanup previous info if we've changed*/
	if (RosterInfo)
		ClearRosterInfo();

	/*set the roster*/
	RosterInfo = NewRoster;

	/*bind to its member changes*/
	RosterInfo->OnTeamMemberAdded.AddDynamic(this, &URosterInfoEntry::NativeOnTeamMemberAdded);
	RosterInfo->OnTeamMemberRemoved.AddDynamic(this, &URosterInfoEntry::NativeOnTeamMemberRemoved);
		
	RefreshRosterInfo();
	
	if (OnRosterInfoChanged.IsBound())
		OnRosterInfoChanged.Broadcast();
}

/*ClearRosterInfo - Cleans up bindings and nulls out references*/
void URosterInfoEntry::ClearRosterInfo()
{
	if (!RosterInfo)
		return;

	RosterInfo->OnTeamMemberAdded.RemoveDynamic(this, &URosterInfoEntry::NativeOnTeamMemberAdded);
	RosterInfo->OnTeamMemberRemoved.RemoveDynamic(this, &URosterInfoEntry::NativeOnTeamMemberRemoved);
	
	RosterInfo = nullptr;
}

void URosterInfoEntry::RefreshRosterInfo()
{

	BP_OnRosterChanged();
}

ARosterInfo* URosterInfoEntry::GetRosterInfo(FString Team)
{
	if (!IsValid(GetGameInstance()))
		return nullptr;

	if (UMissionSubsystem* MSS = GetGameInstance()->GetSubsystem<UMissionSubsystem>())
	{
		return MSS->GetRosterByName(Team);
	}

	return nullptr;
}

void URosterInfoEntry::NativeOnTeamMemberAdded(ACharacterInfo* NewTeamMember)
{
	BP_OnTeamMemberAdded(NewTeamMember);
	RefreshRosterInfo();
}

void URosterInfoEntry::NativeOnTeamMemberRemoved(ACharacterInfo* RemovedTeamMember)
{	
	BP_OnTeamMemberRemoved(RemovedTeamMember);
	RefreshRosterInfo();
}

TArray<class ACharacterInfo*> URosterInfoEntry::GetTeamMembers()
{
	if(!RosterInfo)
		return TArray<class ACharacterInfo*>();

	return RosterInfo->GetTeamMembers();
}

void URosterInfoEntry::AddTeamMember(ACharacterInfo* TeamMember)
{
	if (!TeamMember || !RosterInfo)
		return;

	RosterInfo->AddTeamMember(TeamMember);
	NativeOnTeamMemberAdded(TeamMember);
}
