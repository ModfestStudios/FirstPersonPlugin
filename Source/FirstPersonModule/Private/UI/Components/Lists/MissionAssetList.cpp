// copyrighted 2024-2025 - Modfest Studios & Micah A. Parker


#include "UI/Components/Lists/MissionAssetList.h"

/*engine*/
#include "Engine/GameInstance.h"

void UMissionAssetList::RefreshMissionList()
{
	if (UMissionSubsystem* MSS = GetGameInstance()->GetSubsystem<UMissionSubsystem>())
	{
		if (MissionsFilter == EMissionListType::AllMissions)
		{
			Missions = MSS->GetAllMissions();
		}
	}

	BP_OnRefreshMissionList();
}
