// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Blueprint/IUserObjectListEntry.h"
#include "RosterInfoEntry.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnRosterInfoChanged);


/**
 * 
 */
UCLASS(abstract)
class FIRSTPERSONMODULE_API URosterInfoEntry : public UUserWidget, public IUserObjectListEntry
{
	GENERATED_BODY()
public:

	/*if set - will try to pull the team based off this name*/
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Roster")
		bool bAutoFindTeam;
	
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Roster", meta = (EditCondition = "bAutoFindTeam"))
		FString TeamName = "Red Team";
	UPROPERTY(BlueprintReadOnly, Category = "Roster")
		class ARosterInfo* RosterInfo;


	/*events*/
	UPROPERTY(BlueprintAssignable)
		FOnRosterInfoChanged OnRosterInfoChanged;


	//=============================================================================================================================
	//==========================================================FUNCTIONS==========================================================
	//=============================================================================================================================
public:


	virtual void NativeConstruct() override;
	/*integration support for List UMG Widgets that automatically calls SetCharacterInfo() for us*/
	virtual void NativeOnListItemObjectSet(UObject* ListItemObject) override;

	UFUNCTION(BlueprintCallable, Category = "Roster")
		virtual void SetRosterInfo(class ARosterInfo* NewRoster);
	UFUNCTION(BlueprintCallable, Category = "Roster")
		virtual void ClearRosterInfo();
	UFUNCTION(BlueprintCallable, Category = "Roster")
		virtual void RefreshRosterInfo();
	UFUNCTION()
		class ARosterInfo* GetRosterInfo(FString Team);
	
	UFUNCTION(BlueprintImplementableEvent, Category = "Roster", meta = (DisplayName = "On Roster Changed"))
		void BP_OnRosterChanged();


	UFUNCTION()
		void NativeOnTeamMemberAdded(class ACharacterInfo* NewTeamMember);
	UFUNCTION(BlueprintImplementableEvent, Category = "Roster", meta = (DisplayName = "On Team Member Added"))
		void BP_OnTeamMemberAdded(class ACharacterInfo* NewTeamMember);
	
	UFUNCTION()
		void NativeOnTeamMemberRemoved(class ACharacterInfo* RemovedTeamMember);
	UFUNCTION(BlueprintImplementableEvent, Category = "Roster", meta = (DisplayName = "On Team Member Removed"))
		void BP_OnTeamMemberRemoved(class ACharacterInfo* RemovedTeamMember);

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Roster")
		TArray<class ACharacterInfo*> GetTeamMembers();
	UFUNCTION(BlueprintCallable, Category = "Roster")
		virtual void AddTeamMember(class ACharacterInfo* TeamMember);	


	
};
