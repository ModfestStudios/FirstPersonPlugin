// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "RosterInfoList.generated.h"



/**
 * 
 */
UCLASS()
class FIRSTPERSONMODULE_API URosterInfoList : public UUserWidget
{
	GENERATED_BODY()
public:


	//===============================================================================================================================
	//===========================================================FUNCTIONS===========================================================
	//===============================================================================================================================
public:
	UFUNCTION(BlueprintCallable, Category = "Roster")
		virtual void RefreshRoster();
	UFUNCTION(BlueprintImplementableEvent, Category = "Roster", meta=(DisplayName="On Refresh Roster"))
		void BP_OnRefreshRoster();



	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Roster")
		TArray<class ARosterInfo*> GetTeamRosters();

	
};
