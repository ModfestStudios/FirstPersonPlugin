// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "MissionSubsystem.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FTeamRosterChanged);

UENUM(BlueprintType)
enum class EMissionListType : uint8
{
	AllMissions,
	Training,
	Stealth
};

UENUM(BlueprintType)
enum class EMissionType : uint8
{
	Training,
	Assault,
	Recon,
	Stealth
};

/**
 * 
 */
UCLASS()
class FIRSTPERSONMODULE_API UMissionSubsystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()
public:



	//=======================================================================================================================================
	//===============================================================FUNCTIONS===============================================================
	//=======================================================================================================================================


	/** Implement this for initialization of instances of the system */
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;

	/** Implement this for deinitialization of instances of the system */
	virtual void Deinitialize() override;



	/*missions*/
private:
	UPROPERTY() //active mission established when the mission actually loads
		class AMissionInfo* CurrentMission; 
	/*list of missions to select froms*/
	UPROPERTY()
		TArray<class UMissionAsset*> Missions;


	UPROPERTY() //selected in the UI
		class UMissionAsset* SelectedMission;
	UPROPERTY() //hovered in the UI
		class UMissionAsset* HoveredMission;

	/*rosters*/
private:
	UPROPERTY()
		TArray<class ARosterInfo*> TeamRosters;

	/*team members*/
	/*UPROPERTY()
		class ACharacterInfo* HoveredTeamMember;
	UPROPERTY()
		class ACharacterInfo* SelectedTeamMember;*/


	/*events*/
public:
	UPROPERTY(BlueprintAssignable)
		FTeamRosterChanged OnTeamRosterChanged;

	//=================================================================================================
	//============================================FUNCTIONS============================================
	//=================================================================================================

	//============================================
	//==============MISSION PLANNING==============
	//============================================

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Missions|Current Mission")
		class AMissionInfo* GetCurrentMission();
	UFUNCTION(BlueprintCallable, Category = "Missions|Current Mission")
		virtual void SetCurrentMission(class AMissionInfo* Mission);
	UFUNCTION(BlueprintCallable, Category = "Missions|Current Mission")
		virtual void ClearCurrentMission();

	UFUNCTION(BlueprintCallable, Category = "Missions|Mission List")
		TArray<class UMissionAsset*> GetAllMissions();
	UFUNCTION(BlueprintCallable, Category = "Missions|Mission List")
		TArray<class UMissionAsset*> GetMissionsByType(EMissionListType FilterType);


	UFUNCTION(BlueprintCallable, Category = "Missions|Mission List")
		virtual void AddMission(class UMissionAsset* Mission);


	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Missions|UI")
		class UMissionAsset* GetSelectedMission();
	UFUNCTION(BlueprintCallable, Category = "Missions|UI")
		virtual void SetSelectedMission(class UMissionAsset* Mission);
	UFUNCTION(BlueprintCallable, Category = "Missions|UI")
		virtual void UnselectMission(class UMissionAsset* Mission);
	UFUNCTION(BlueprintCallable, Category = "Missions|UI")
		virtual void ClearSelectedMission();
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Missions|UI")
		class UMissionAsset* GetHoveredMission();
	UFUNCTION(BlueprintCallable, Category = "Missions|UI")
		virtual void SetHoveredMission(class UMissionAsset* Mission);
	UFUNCTION(BlueprintCallable, Category = "Missions|UI")
		virtual void ClearHoveredMission();


	//====================================
	//===============MISSION==============
	//====================================

	

	/*loads the mission in the background asynchronously without enabling it for smoother streaming*/
	UFUNCTION(BlueprintCallable, Category = "Missions|Current Mission")
		virtual void LoadMissionLevelAsync();

	/*begins moving playings into the "transition map" and unloads the previous map*/
	UFUNCTION(BlueprintCallable, Category = "Missions|Current Mission")
		virtual void BeginMissionTravel();
	/*event called after the mission has fully loaded in the background*/
	UFUNCTION()
		virtual void OnMissionLevelLoaded();

	/*activates the mission and notifies gamemode/blueprints*/
	UFUNCTION(BlueprintCallable, Category = "Missions|Current Mission")
		virtual void StartMission();


	UFUNCTION(BlueprintCallable, Category = "Missions")
		virtual void SortMissionsAlphabetically(TArray<UMissionAsset*>& InMissions);

	UFUNCTION()
		virtual void LoadDefaultMissions();

	//====================================
	//===============ROSTER===============
	//====================================
public:
	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly, Category = "Missions|Team Roster")
		class ARosterInfo* AddNewTeamRoster(FString TeamName);
	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly, Category = "Missions|Team Roster")
		void AddTeamRoster(class ARosterInfo* NewTeamRoster);

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Missions|Team Roster")
		TArray<class ARosterInfo*> GetTeamRosters();
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Missions|Team Roster")
		class ARosterInfo* GetRosterByName(FString TeamName);

	//==========================
	//=======TEAM MEMBERS=======
	//==========================
	UFUNCTION(BlueprintCallable, Category = "Missions|Team Roster|Members")
		virtual void AssignTeamMember(class ARosterInfo* Roster, class ACharacterInfo* TeamMember);


	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Missions|Team Roster|Members")
		class ACharacterInfo* GetHoveredTeamMember();
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Missions|Team Roster|Members")
		class ACharacterInfo* GetSelectedTeamMember();


	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Missions|Team Roster|Members")
		TArray<class ACharacterInfo*> GetAssignedTeamMembers();
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Missions|Team Roster|Members")
		TArray<class ACharacterInfo*> GetUnassignedTeamMembers();

	UFUNCTION(BlueprintCallable, Category = "Missions|Team Roster|Members")
		virtual void SetHoveredTeamMember(class ACharacterInfo* TeamMember);
	UFUNCTION(BlueprintCallable, Category = "Missions|Team Roster|Members")
		virtual void SetSelectedTeamMember(class ACharacterInfo* TeamMember);
	UFUNCTION(BlueprintCallable, Category = "Missions|Team Roster|Members")
		virtual void UnselectTeamMember(class ACharacterInfo* TeamMember = nullptr);


	//==========================
	//==========EVENTS==========
	//==========================

	UFUNCTION(BlueprintCallable, Category = "Missions|Events")
		virtual void NotifyTeamChange();



	
};
