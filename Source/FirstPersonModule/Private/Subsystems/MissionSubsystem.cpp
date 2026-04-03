// Fill out your copyright notice in the Description page of Project Settings.


#include "Subsystems/MissionSubsystem.h"

/*asset registry (primary data asset)*/
#include "AssetRegistry/AssetRegistryModule.h"
#include "AssetRegistry/IAssetRegistry.h"

/*characters*/
#include "Characters/CharacterInfo.h"
#include "Subsystems/CharacterSubsystem.h"

/*engine*/
#include "Engine/GameInstance.h"
#include "Engine/World.h"

/*roster & teamembers*/
#include "Rosters/RosterInfo.h"

/*missions & maps*/
#include "Maps/MapAsset.h"
#include "Missions/MissionAsset.h"
#include "Missions/MissionInfo.h"

/*level streaming*/
#include "Engine/StreamableManager.h"
#include "Engine/AssetManager.h"

/*utilities*/
#include "..\Logging.h"


/*Used to Initialize all the MissionAssets into the MissionSubsystem (Loads them).
* To get Missions to load - you must first add them into the Project Settings.Asset Manager 
* For Data Assets being created via Misc.Data Assets (or the Missions tab) must have "Has Blueprints Classes" unchecked
*  
*
*/
void UMissionSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);
	LoadDefaultMissions();
}

void UMissionSubsystem::Deinitialize()
{

}

AMissionInfo* UMissionSubsystem::GetCurrentMission()
{
	return CurrentMission;
}

void UMissionSubsystem::SetCurrentMission(AMissionInfo* Mission)
{
	CurrentMission = Mission;
}

void UMissionSubsystem::ClearCurrentMission()
{
	CurrentMission = nullptr;
}

TArray<class UMissionAsset*> UMissionSubsystem::GetAllMissions()
{	
	return Missions;
}

TArray<class UMissionAsset*> UMissionSubsystem::GetMissionsByType(EMissionListType FilterType)
{
	TArray<UMissionAsset*> FilteredMissions;

	for (UMissionAsset* Mission : Missions)
	{
		if (UEnum::GetValueAsString(FilterType) == UEnum::GetValueAsString(Mission->MissionType))
			FilteredMissions.Add(Mission);
	}

	SortMissionsAlphabetically(FilteredMissions);

	return FilteredMissions;
}

void UMissionSubsystem::AddMission(UMissionAsset* Mission)
{
	Missions.AddUnique(Mission);
}

UMissionAsset* UMissionSubsystem::GetSelectedMission()
{
	return SelectedMission;
}

void UMissionSubsystem::SetSelectedMission(UMissionAsset* Mission)
{	
	SelectedMission = Mission;
}

void UMissionSubsystem::UnselectMission(UMissionAsset* Mission)
{
	if (SelectedMission == Mission)
		ClearSelectedMission();
}

void UMissionSubsystem::ClearSelectedMission()
{
	SelectedMission = nullptr;
}

UMissionAsset* UMissionSubsystem::GetHoveredMission()
{
	return HoveredMission;
}

void UMissionSubsystem::SetHoveredMission(UMissionAsset* Mission)
{
	HoveredMission = Mission;
}

void UMissionSubsystem::ClearHoveredMission()
{
	HoveredMission = nullptr;
}




void UMissionSubsystem::LoadMissionLevelAsync()
{
	/*safety check*/
	if (!SelectedMission)
	{
		UE_LOG(LogMission, Warning, TEXT("UMissionSubsystem::LoadMissionLevelAsync() - !!! NO MISSION IS SELECTED !!!"));
		return;
	}
	
	/*load map*/
	if (!SelectedMission->Level.IsValid())
	{
		FStreamableManager& StreamManager = UAssetManager::GetStreamableManager();

		StreamManager.RequestAsyncLoad(SelectedMission->Level.ToSoftObjectPath(),
			FStreamableDelegate::CreateUObject(this, &UMissionSubsystem::OnMissionLevelLoaded));
	}
	/*already loaded*/
	else
	{
		OnMissionLevelLoaded();
	}

	
}

void UMissionSubsystem::BeginMissionTravel()
{
	UE_LOG(LogMission, Log, TEXT("UMissionSubsystem::BeginMissionTravel()"));

	if (!SelectedMission || !SelectedMission->Level.IsValid())
	{
		UE_LOG(LogMission, Error, TEXT("UMissionSubsystem::BeginMissionTravel() - No valid map found"));
		return;
	}
	
	FString MapName = SelectedMission->Level->GetOutermost()->GetFName().ToString();
	FString TravelURL = MapName;

	UE_LOG(LogMission, Log, TEXT("Mission Travel Destination: %s"),*TravelURL);

	GetWorld()->ServerTravel(TravelURL, true);

}

void UMissionSubsystem::OnMissionLevelLoaded()
{
	UE_LOG(LogMission, Log, TEXT("UMissionSubsystem::OnMissionLevelLoaded()"));
}

void UMissionSubsystem::StartMission()
{

}

void UMissionSubsystem::SortMissionsAlphabetically(TArray<UMissionAsset*>& InMissions)
{
	InMissions.Sort(UMissionAsset::SortAlphabetically);
}

void UMissionSubsystem::LoadDefaultMissions()
{
	/*gather all the missions and load them*/
	UAssetManager& AssetManager = UAssetManager::Get();

	FPrimaryAssetId MissionAssetId("Mission");
	TArray<FPrimaryAssetId> MissionIds;
	AssetManager.GetPrimaryAssetIdList(UMissionAsset::PrimaryAssetType, MissionIds);

	Missions.Reset();

	// Load them all (sync)
	if (TSharedPtr<FStreamableHandle> Handle = AssetManager.LoadPrimaryAssets(MissionIds))
	{
		Handle->WaitUntilComplete();
	}

	// Now collect loaded objects
	for (const FPrimaryAssetId& Id : MissionIds)
	{
		if (UObject* Loaded = AssetManager.GetPrimaryAssetObject(Id))
		{
			if (UMissionAsset* Mission = Cast<UMissionAsset>(Loaded))
			{
				Missions.Add(Mission);
			}
		}
	}

	SortMissionsAlphabetically(Missions);
}


ARosterInfo* UMissionSubsystem::AddNewTeamRoster(FString TeamName)
{
	/*initialize*/
	ARosterInfo* NewTeam = nullptr;

	/*server check*/
	if (GetWorld() && GetWorld()->GetNetMode() < NM_Client)
	{
		/*spawn the new Roster*/
		NewTeam = GetWorld()->SpawnActor<ARosterInfo>();
		
		if (NewTeam)
		{
			NewTeam->TeamName = TeamName;

			/*add to team*/
			TeamRosters.Add(NewTeam);			
		}		
	}

	NotifyTeamChange();

	return NewTeam;
}

void UMissionSubsystem::AddTeamRoster(ARosterInfo* NewTeamRoster)
{
	if (!NewTeamRoster)
		return;

	TeamRosters.AddUnique(NewTeamRoster);
	NotifyTeamChange();
}

TArray<class ARosterInfo*> UMissionSubsystem::GetTeamRosters()
{
	return TeamRosters;	
}

ARosterInfo* UMissionSubsystem::GetRosterByName(FString TeamName)
{
	for (ARosterInfo* Team : TeamRosters)
	{
		if (Team->TeamName == TeamName)
			return Team;
	}

	return nullptr;
}

void UMissionSubsystem::AssignTeamMember(ARosterInfo* Roster, ACharacterInfo* TeamMember)
{
	if (!Roster || !TeamMember)
		return;

	Roster->AddTeamMember(TeamMember);
	NotifyTeamChange();
}

ACharacterInfo* UMissionSubsystem::GetHoveredTeamMember()
{
	if (UCharacterSubsystem* CSS = GetGameInstance()->GetSubsystem<UCharacterSubsystem>())
		return CSS->GetHoveredCharacter();

	return nullptr;
}

ACharacterInfo* UMissionSubsystem::GetSelectedTeamMember()
{
	if (UCharacterSubsystem* CSS = GetGameInstance()->GetSubsystem<UCharacterSubsystem>())
		return CSS->GetSelectedCharacter();

	return nullptr;
}

TArray<class ACharacterInfo*> UMissionSubsystem::GetAssignedTeamMembers()
{
	TArray<ACharacterInfo*> AssignedTeamMembers;
	for (ARosterInfo* TeamRoster : TeamRosters)
	{
		for (ACharacterInfo* TeamMember : TeamRoster->GetTeamMembers())
		{
			AssignedTeamMembers.Add(TeamMember);
		}
	}
	
	return AssignedTeamMembers;
}

TArray<class ACharacterInfo*> UMissionSubsystem::GetUnassignedTeamMembers()
{
	TArray<ACharacterInfo*> UnassignedTeamMembers;
	if (UCharacterSubsystem* CSS = GetGameInstance()->GetSubsystem<UCharacterSubsystem>())
	{
		UnassignedTeamMembers = CSS->GetUnselectedCharacters();

		// Remove all that are already assigned to a roster
		UnassignedTeamMembers.RemoveAll([](ACharacterInfo* CharInfo)
			{
				return CharInfo && CharInfo->IsAssignedToRoster();
			});
	}

	return UnassignedTeamMembers;
}

void UMissionSubsystem::SetHoveredTeamMember(ACharacterInfo* TeamMember)
{
	if (UCharacterSubsystem* CSS = GetGameInstance()->GetSubsystem<UCharacterSubsystem>())
	{
		if (TeamMember != nullptr && CSS->GetHoveredCharacter() != TeamMember)
		{
			CSS->SetHoveredCharacter(TeamMember);
		}
	}	
}

void UMissionSubsystem::SetSelectedTeamMember(ACharacterInfo* TeamMember)
{
	if (UCharacterSubsystem* CSS = GetGameInstance()->GetSubsystem<UCharacterSubsystem>())
	{
		if (TeamMember != nullptr && CSS->GetSelectedCharacter() != TeamMember)
		{
			CSS->SetSelectedCharacter(TeamMember);
		}
	}
}

void UMissionSubsystem::UnselectTeamMember(ACharacterInfo* TeamMember)
{
	if (UCharacterSubsystem* CSS = GetGameInstance()->GetSubsystem<UCharacterSubsystem>())
	{
		CSS->UnselectCharacter(TeamMember);
	}	
	
}

void UMissionSubsystem::NotifyTeamChange()
{
	if (OnTeamRosterChanged.IsBound())
		OnTeamRosterChanged.Broadcast();
}
