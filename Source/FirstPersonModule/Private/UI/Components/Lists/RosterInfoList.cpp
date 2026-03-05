// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/Components/Lists/RosterInfoList.h"
#include "Subsystems/MissionSubsystem.h"

/*engine*/
#include "Engine/GameInstance.h"

TArray<class ARosterInfo*> URosterInfoList::GetTeamRosters()
{
	if (UMissionSubsystem* MSS = GetGameInstance()->GetSubsystem<UMissionSubsystem>())
	{
		return MSS->GetTeamRosters();
	}

	return TArray<class ARosterInfo*>();
}

void URosterInfoList::RefreshRoster()
{

	BP_OnRefreshRoster();
}

