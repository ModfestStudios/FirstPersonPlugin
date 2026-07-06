// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerState.h"
#include "FirstPersonPlayerState.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnReadyStateChanged, AFirstPersonPlayerState*, PlayerState, bool, bReady);

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
	UPROPERTY()
		class ARosterInfo* AssignedRoster; 
	/*the character state this player has ownerhship of*/
	UPROPERTY()
		class ACharacterInfo* CharacterInfo;

public:
	UPROPERTY()
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
		FOnReadyStateChanged OnReadyStateChanged;
	


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


	UFUNCTION()
		bool IsAssignedToRoster();

public:

	UFUNCTION(BlueprintCallable, Category = "Player State|Character")
		virtual void SetCharacterInfo(class ACharacterInfo* NewCharacterInfo);
	UFUNCTION(BlueprintCallable, Category = "Player State|Roster")
		virtual void SetRosterInfo(class ARosterInfo* NewRosterInfo);
	

};
