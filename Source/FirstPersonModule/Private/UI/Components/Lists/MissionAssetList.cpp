// copyrighted 2024-2025 - Modfest Studios & Micah A. Parker


#include "UI/Components/Lists/MissionAssetList.h"
#include "Missions/MissionAsset.h"

/*engine*/
#include "Engine/GameInstance.h"

void UMissionAssetList::NativeConstruct()
{
	Super::NativeConstruct();

	RefreshMissionList();
}

void UMissionAssetList::RefreshMissionList()
{

	if (bAutoPopulateMissions)
	{
		Missions.Empty();

		if (UMissionSubsystem* MSS = GetGameInstance()->GetSubsystem<UMissionSubsystem>())
		{
			if (MissionsFilter == EMissionListType::AllMissions)
			{
				if (bExcludeTrainingMissions)
				{
					for (UMissionAsset* Mission : MSS->GetAllMissions())
					{
						if (Mission->MissionType != EMissionType::Training)
							Missions.Add(Mission);
					}
				}

				else
				{
					Missions = MSS->GetAllMissions();
				}
			}
			else
			{
				Missions = MSS->GetMissionsByType(MissionsFilter);
			}

			//MSS->SortMissionsAlphabetically(Missions);
		}
	}

	

	BP_OnRefreshMissionList();
}
