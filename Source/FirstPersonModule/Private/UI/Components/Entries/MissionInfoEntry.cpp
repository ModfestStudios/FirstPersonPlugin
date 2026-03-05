// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/Components/Entries/MissionInfoEntry.h"
#include "Missions/MissionAsset.h"
#include "Subsystems/MissionSubsystem.h"

/*engine*/
#include "Engine/GameInstance.h"

void UMissionInfoEntry::NativeOnListItemObjectSet(UObject* ListItemObject)
{
	if (UMissionAsset* InMission = Cast<UMissionAsset>(ListItemObject))
		Mission = InMission;

	Execute_OnListItemObjectSet(Cast<UObject>(this), ListItemObject);
}

void UMissionInfoEntry::NativeOnItemSelectionChanged(bool bIsSelected)
{
	if (bIsSelected)
		SelectMission();
	else
		UnselectMission();

	Execute_BP_OnItemSelectionChanged(Cast<UObject>(this), bIsSelected);
}

void UMissionInfoEntry::SelectMission()
{
	if (!Mission)
		return;

	if (UMissionSubsystem* MSS = GetGameInstance()->GetSubsystem<UMissionSubsystem>())
		MSS->SetSelectedMission(Mission);
}

void UMissionInfoEntry::UnselectMission()
{
	if (!Mission)
		return;

	if (UMissionSubsystem* MSS = GetGameInstance()->GetSubsystem<UMissionSubsystem>())
		MSS->UnselectMission(Mission);
}
