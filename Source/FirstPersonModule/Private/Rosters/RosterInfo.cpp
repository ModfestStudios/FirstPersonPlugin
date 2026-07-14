// Fill out your copyright notice in the Description page of Project Settings.


#include "Rosters/RosterInfo.h"
#include "Characters/CharacterInfo.h"

/*players*/
#include "Players/FirstPersonPlayerState.h"

/*engine*/
#include "Engine/GameInstance.h"

/*subsystems*/
#include "Subsystems/MissionSubsystem.h"

/*network*/
#include "Net/UnrealNetwork.h"


void ARosterInfo::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ARosterInfo, Players);
	DOREPLIFETIME(ARosterInfo, TeamMembers);
}


ARosterInfo::ARosterInfo()
{
	bReplicates = true;
	bAlwaysRelevant = true;
	bOnlyRelevantToOwner = false;	
}

void ARosterInfo::AddTeamMember(ACharacterInfo* Member)
{
	if (!Member || TeamMembers.Contains(Member))
		return;
	
	/*clear out old assignment*/
	if (Member->IsAssignedToRoster())
		ClearOldRoster(Member);

	/*assign to us*/
	TeamMembers.AddUnique(Member);	
	Member->SetAssignedRoster(this);

	NotifyTeamMemberAdded(Member);

	if (GetMissionSubsystem())
	{
		GetMissionSubsystem()->UnselectTeamMember(Member);
		GetMissionSubsystem()->NotifyTeamChange();
	}	
		
}

void ARosterInfo::RemoveTeamMember(ACharacterInfo* Member)
{
	if (!Member || !TeamMembers.Contains(Member))
		return;

	TeamMembers.Remove(Member);
	Member->ClearAssignedRoster();

	NotifyTeamMemberRemoved(Member);

	if (GetMissionSubsystem())
	{
		GetMissionSubsystem()->UnselectTeamMember(Member);
		GetMissionSubsystem()->NotifyTeamChange();
	}
		
	
}

void ARosterInfo::ClearOldRoster(ACharacterInfo* TeamMember)
{
	if (!TeamMember || !TeamMember->AssignedRoster)
		return;

	ARosterInfo* OldRoster = TeamMember->AssignedRoster;
	OldRoster->RemoveTeamMember(TeamMember);

}

TArray<class ACharacterInfo*> ARosterInfo::GetTeamMembers()
{
	return TeamMembers;
}

TArray<class AFirstPersonPlayerState*> ARosterInfo::GetPlayers()
{
	return Players;
}

void ARosterInfo::AddPlayer(AFirstPersonPlayerState* Player)
{
	if (!Players.Contains(Player))
	{
		Players.Add(Player);
		Player->SetRosterInfo(this);

		NotifyPlayerAdded(Player);
	}	
}

void ARosterInfo::RemovePlayer(AFirstPersonPlayerState* Player)
{
	if (Players.Contains(Player))
	{
		Players.Remove(Player);

		NotifyPlayerRemoved(Player);
	}
}

bool ARosterInfo::HasPlayer(AFirstPersonPlayerState* Player)
{
	return Players.Contains(Player);
}

UMissionSubsystem* ARosterInfo::GetMissionSubsystem()
{
	if (MissionSubsystem == nullptr)
		MissionSubsystem = GetGameInstance()->GetSubsystem<UMissionSubsystem>();

	return MissionSubsystem;
}

void ARosterInfo::NotifyTeamMemberAdded(class ACharacterInfo* NewTeamMember)
{
	if (OnTeamMemberAdded.IsBound())
		OnTeamMemberAdded.Broadcast(NewTeamMember);
}

void ARosterInfo::NotifyTeamMemberRemoved(class ACharacterInfo* RemovedTeamMember)
{
	if (OnTeamMemberRemoved.IsBound())
		OnTeamMemberRemoved.Broadcast(RemovedTeamMember);
}

void ARosterInfo::NotifyPlayerAdded(AFirstPersonPlayerState* Player)
{
	if(OnPlayerAdded.IsBound())
		OnPlayerAdded.Broadcast(Player);
}

void ARosterInfo::NotifyPlayerRemoved(AFirstPersonPlayerState* Player)
{
	if(OnPlayerRemoved.IsBound())
		OnPlayerRemoved.Broadcast(Player);
}
