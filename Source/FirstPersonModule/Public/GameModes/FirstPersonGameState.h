// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameStateBase.h"
#include "FirstPersonGameState.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnPlayerReadyStateChanged, class AFirstPersonPlayerState*, PlayerState, bool, bIsReady);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnPlayerJoins, class AFirstPersonPlayerState*, PlayerState);

/**
 * 
 */
UCLASS()
class FIRSTPERSONMODULE_API AFirstPersonGameState : public AGameStateBase
{
	friend class AFirstPersonGame;
	friend class UServerSubsystem;

	GENERATED_BODY()
public:	

	/*property used specifically to replicate info between clients and server*/
	UPROPERTY(Replicated, ReplicatedUsing = OnRep_CharacterListChanged)
		TArray<class ACharacterInfo*> CharacterList;


	/*cinema subsystem*/
public:
	UPROPERTY(Replicated, ReplicatedUsing = OnRep_CSSViewTargetChanged)
		AActor* CSS_ViewTarget;


private:
	//UPROPERTY(Replicated, ReplicatedUsing = OnRep_CurrentMap)
	//	class UMapAsset* CurrentMap;
	//UPROPERTY(Replicated, ReplicatedUsing = OnRep_NextMap)
	//	class UMapAsset* NextMap;
	//UPROPERTY(Replicated, ReplicatedUsing = OnRep_MapRotation)
	//	TArray<class UMapAsset*> MapRotation;

private:
	/*the level we're currently loading in via streaming*/
	UPROPERTY()
		bool bLoadingLevel = false;
	FTimerHandle LevelStreamHandler;
	UPROPERTY()
		class ULevelStreamingDynamic* LoadingLevelStream;
	UPROPERTY()
		FString LoadingLevelPath = "";



	//======================
	//========EVENTS========
	//======================
public:
	UPROPERTY(BlueprintAssignable)
		FOnPlayerReadyStateChanged OnPlayerReadyStateChanged;
	UPROPERTY(BlueprintAssignable)
		FOnPlayerJoins OnPlayerJoins;
	UPROPERTY(BlueprintAssignable)
		FOnPlayerJoins OnPlayerLeaves;



	//=====================================================================================================================================
	//==============================================================FUNCTIONS==============================================================
	//=====================================================================================================================================



public:
	/*function that automatically syncs the character list to all clients */
	UFUNCTION()
		void SyncCharacterList();
protected:
	UFUNCTION()
		void OnRep_CharacterListChanged();


	//==================================
	//=========CINEMA SUBSYSTEM=========
	//==================================
public:
	UFUNCTION()
		void OnRep_CSSViewTargetChanged();

	//===============================
	//============PLAYERS============
	//===============================
	UFUNCTION()
		virtual void NativeOnPlayerReadyStateChanged(class AFirstPersonPlayerState* PlayerState, bool bIsReady);
	UFUNCTION(BlueprintImplementableEvent, Category = "Players", meta = (DisplayName="OnPlayerReadyStateChanged"))
		void BP_OnPlayerReadyStateChanged(class AFirstPersonPlayerState* PlayerState, bool bIsReady);
	UFUNCTION(BlueprintPure, Category = "Players")
		bool AllPlayersReady();	
	
	/** Add PlayerState to the PlayerArray */
	virtual void AddPlayerState(APlayerState* PlayerState) override;

	/** Remove PlayerState from the PlayerArray. */
	virtual void RemovePlayerState(APlayerState* PlayerState) override;





	//================================
	//========SERVER SUBSYSTEM========
	//================================
public:
	/*Replicated Event whenever the "Current Map" is replicated*/
	UFUNCTION(NetMulticast,Reliable)
		void Multicast_OnCurrentMapUpdated(class UMapAsset* CurrentMap);

	/*Replicated Event whenever the "Next Map" is queued up*/
	UFUNCTION(NetMulticast, Reliable)
		void Multicast_OnNextMapUpdated(class UMapAsset* NextMap);

	UFUNCTION(NetMulticast, Reliable)
		void Multicast_OnMapRotationUpdated(const TArray<class UMapAsset*>& MapRotation);


public:
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Level Streaming")
		int32 GetPlayersLoadingLevelInstanceNum();

	/*level streaming*/
	UFUNCTION(NetMulticast, Reliable)
		void Multicast_LoadLevelInstance(const FSoftObjectPath& LevelPath, FVector Location = FVector(0,0,0), FRotator Rotation = FRotator::ZeroRotator);	
	UFUNCTION()
		void CheckLevelLoaded();
	/*event - called automatically by CheckLevelLoad() on the local machine which is a timer activated by Multicast_LoadLevelInstance*/
	UFUNCTION()
		virtual void OnLevelInstanceLoadFinished();
	/*triggered after a client finishes loading in a new Level Intance pushed by the server*/
	UFUNCTION(BlueprintImplementableEvent,Category = "Levels",meta=(DisplayName = "On Level Instance Loaded Finished"))
		void BP_OnLevelInstanceLoadFinished();


	//===============================
	//===========UTILITIES===========
	//===============================
protected:
	class AFirstPersonLevelScript* GetLevelScript();
	class UServerSubsystem* GetServerSubsystem();
	class UMapSubsystem* GetMapSubsystem();


	
};
