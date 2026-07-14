// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerState.h"
#include "FirstPersonPlayerState.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnAssignedRosterChanged,AFirstPersonPlayerState*, PlayerState,class ARosterInfo*, NewRoster);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnReadyStateChanged, AFirstPersonPlayerState*, PlayerState, bool, bReady);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnLoadoutChanged, AFirstPersonPlayerState*, PlayerState, class AInventoryLoadout*, PlayerLoadout);

/**
 * 
 */
UCLASS()
class FIRSTPERSONMODULE_API AFirstPersonPlayerState : public APlayerState
{
	GENERATED_BODY()
public:


private:
	/*ready state indicating this player is ready to gooooooooo (typically used for lobbies)*/
	UPROPERTY(Replicated, ReplicatedUsing="OnRep_PlayerReadyStateChanged")
		bool bReady;

private:
	
	/*the roster this player is assigned to*/
	UPROPERTY(Replicated, ReplicatedUsing="OnRep_AssignedRoster")
		class ARosterInfo* AssignedRoster; 
	/*the character state this player has ownerhship of*/
	UPROPERTY()
		class ACharacterInfo* CharacterInfo;

private:
	UPROPERTY(Replicated, ReplicatedUsing = "OnRep_Loadout")
		class AInventoryLoadout* Loadout;

public:
	/*flag on whether or not this player is still loading a Level Instance Dynamic map*/
	UPROPERTY(Replicated, BlueprintReadOnly, Category = "Level Streaming")
		bool bLevelInstanceLoading = false;
	UPROPERTY(Replicated, BlueprintReadOnly, Category = "Level Streaming")
		float LevelInstanceLoadPercent = 0.0f;


	/*EVENTS*/
public:
	UPROPERTY(BlueprintAssignable)
		FOnAssignedRosterChanged OnAssignedRosterChanged;
	UPROPERTY(BlueprintAssignable)
		FOnReadyStateChanged OnReadyStateChanged;
	UPROPERTY(BlueprintAssignable)
		FOnLoadoutChanged OnLoadoutChanged;
	


	//===================================================================================================================================
	//=============================================================FUNCTIONS=============================================================
	//===================================================================================================================================

public:
	UFUNCTION(BlueprintCallable, Category = "Player State|Ready State")
		bool IsPlayerReady();
	UFUNCTION(BlueprintCallable, Category = "Player State|Ready State")
		virtual void SetPlayerReady(bool bIsReady);
	UFUNCTION(Server, Reliable, WithValidation)
		virtual void Server_SetPlayerReady(bool bIsReady);		
	UFUNCTION()
		virtual void NotifyReadyStateChanged();


	/*function that is called upon receiving a new ready state*/
	UFUNCTION()
		virtual void OnRep_PlayerReadyStateChanged();


//==========================
//==========ROSTER==========
//==========================
public:
	UFUNCTION()
		bool IsAssignedToRoster();

protected:
	UFUNCTION()
		virtual void OnRep_AssignedRoster();
public:
	UFUNCTION(BlueprintCallable, Category = "Player State|Roster")
		virtual void SetRosterInfo(class ARosterInfo* NewRosterInfo);
	UFUNCTION()
		virtual void NotifyAssignedRosterChanged();

//=============================
//==========CHARACTER==========
//=============================
public:
	UFUNCTION(BlueprintCallable, Category = "Player State|Character")
		virtual void SetCharacterInfo(class ACharacterInfo* NewCharacterInfo);

//=========================
//=========LOADOUT=========
//=========================
public:
	UFUNCTION(BlueprintCallable, Category = "Loadout")
		AInventoryLoadout* GetLoadout();
	UFUNCTION(BlueprintCallable, Category = "Loadout")
		virtual void SetLoadout(class AInventoryLoadout* NewLoadout);
	UFUNCTION(BlueprintCallable, Category = "Loadout")
		virtual void ClearLoadout();
	UFUNCTION()
		virtual void NotifyLoadoutChanged();
	UFUNCTION()
		virtual void OnRep_Loadout();
	

};
