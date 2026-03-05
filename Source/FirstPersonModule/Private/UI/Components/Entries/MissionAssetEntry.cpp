// copyrighted 2024-2025 - Modfest Studios & Micah A. Parker


#include "UI/Components/Entries/MissionAssetEntry.h"
#include "Missions/MissionAsset.h"
#include "Subsystems/MissionSubsystem.h"

/*engine*/
#include "Engine/GameInstance.h"

void UMissionAssetEntry::NativeOnListItemObjectSet(UObject* ListItemObject)
{
	if (UMissionAsset* InMission = Cast<UMissionAsset>(ListItemObject))
		Mission = InMission;

	Execute_OnListItemObjectSet(Cast<UObject>(this), ListItemObject);
}

void UMissionAssetEntry::NativeOnItemSelectionChanged(bool bIsSelected)
{
	if (bIsSelected)
		SelectMission();
	else
		UnselectMission();

	Execute_BP_OnItemSelectionChanged(Cast<UObject>(this), bIsSelected);
}

void UMissionAssetEntry::SelectMission()
{
	if (!Mission)
		return;

	if (UMissionSubsystem* MSS = GetGameInstance()->GetSubsystem<UMissionSubsystem>())
		MSS->SetSelectedMission(Mission);
}

void UMissionAssetEntry::UnselectMission()
{
	if (!Mission)
		return;

	if (UMissionSubsystem* MSS = GetGameInstance()->GetSubsystem<UMissionSubsystem>())
		MSS->UnselectMission(Mission);
}
