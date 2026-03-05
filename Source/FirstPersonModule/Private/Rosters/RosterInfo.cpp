// Fill out your copyright notice in the Description page of Project Settings.


#include "Rosters/RosterInfo.h"
#include "Characters/CharacterInfo.h"

/*engine*/
#include "Engine/GameInstance.h"

/*subsystems*/
#include "Subsystems/MissionSubsystem.h"

ARosterInfo::ARosterInfo()
{
	bReplicates = true;
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
