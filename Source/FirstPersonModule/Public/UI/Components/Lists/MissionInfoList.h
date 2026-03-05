// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Subsystems/MissionSubsystem.h"
#include "MissionInfoList.generated.h"

/**
 * 
 */
UCLASS()
class FIRSTPERSONMODULE_API UMissionInfoList : public UUserWidget
{
	GENERATED_BODY()
public:

	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Missions")
		EMissionListType MissionsFilter;
	UPROPERTY(BlueprintReadOnly, Category = "Missions")
		TArray<class AMissionInfo*> Missions;

	//=========================================================================================================================================
	//================================================================FUNCTIONS================================================================
	//=========================================================================================================================================



	/*UFUNCTION(BlueprintCallable, Category = "Missions")
		virtual void RefreshMissionList();
	UFUNCTION(BlueprintImplementableEvent, Category = "Missions", meta=(DisplayName="On Mission List Refreshed"))
		void BP_OnRefreshMissionList();*/
	
};
