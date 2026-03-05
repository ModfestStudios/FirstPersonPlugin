// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "Engine/Level.h"
#include "FirstPersonGame.generated.h"

/**
 * 
 */
UCLASS()
class FIRSTPERSONMODULE_API AFirstPersonGame : public AGameModeBase
{
	GENERATED_BODY()
public:


	/*if this is enabled - then the server will accept a "Next Map" change from Clients via RPC call in PlayerController*/
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Map Selection")
		bool bAllowClientsToSetNextMap = false;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Classes")
		TSubclassOf<class UGameLobbyWidget> LobbyWidget;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Classes")
		TSubclassOf<UUserWidget> EscapeMenuWidgetClass;

	/*widget for displaying Server Info to Clients*/
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Classes")
		TSubclassOf<UUserWidget> ServerInfoWidgetClass;

	/*class used to store character info per-pawn/player*/
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Classes")
		TSubclassOf<class ACharacterInfo> CharacterInfoClass;
	

	/*when enabled - character death is logged and saved*/
	UPROPERTY(EditDefaultsOnly, Category = "Characters|Spawning")
		bool bPermanentDeath = false;



	//=============================================================================================================================
	//==========================================================FUNCTIONS==========================================================
	//=============================================================================================================================
	AFirstPersonGame();




protected:

	
	virtual void HandleStartingNewPlayer_Implementation(APlayerController* NewPlayer) override;
	virtual void RestartPlayer(AController* NewPlayer) override;


public:
	/*gets all Player Controllers who are of type First Person Controller*/
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Players")
		TArray<class AFirstPersonPlayerController*> GetAllPlayers();

	/*returns true if there is no player alive (factors in spectators or players not active in game)*/
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Players")
		bool AllPlayersDead();

	/*function called automatically whenever a player dies - allowing us to do things in response*/
	UFUNCTION()
		virtual void OnPlayerDeath(AFirstPersonCharacter* Character, AActor* DamageCauser, AController* DamageInstigator,const class UDamageType* DamageType);
protected:
	UFUNCTION(BlueprintImplementableEvent, Category = "GameMode|Events", meta = (DisplayName = "On Player Death"))
		void BP_OnPlayerDeath(AFirstPersonCharacter* Character, AActor* DamageCauser, AController* DamageInstigator, const class UDamageType* DamageType);


	virtual void PostLogin(APlayerController* NewPlayer) override;

	//============================
	//=========CHARACTERS=========
	//============================
	UFUNCTION(BlueprintCallable, Category = "GameMode|Characters")
		virtual class ACharacterInfo* SpawnCharacterInfoForPlayer(class APlayerController* Player);


public:
	UFUNCTION(BlueprintCallable, Category = "Classes")
		TSubclassOf<ASpectatorPawn> GetSpectatorClass() { return SpectatorClass; }


	//===========================
	//=======MAP SELECTION=======
	//===========================
	
	UFUNCTION()
		virtual void OnCurrentMapUpdated(class UMapAsset* MapAsset);
	UFUNCTION()
		virtual void OnNextMapUpdated(class UMapAsset* MapAsset);
	UFUNCTION()
		virtual void OnMapRotationUpdated(TArray<class UMapAsset*> MapRotation);


	//=============================
	//=======LEVEL STREAMING=======
	//=============================
	
	
	/*add additional actors we want to save during our transitions*/
	virtual void GetSeamlessTravelActorList(bool bToEntry, TArray<AActor*>& ActorList) override;

	UFUNCTION(BlueprintCallable, Category = "Levels")
		class ULevelStreamingDynamic* GetLevelInstance();
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Levels")
		bool HasLevelInstanceLoadedForAllPlayers();

	/*this function will trigger the loading a level instance (without it needing to be pre-defined in the map) for all clients (including server host)*/
	UFUNCTION(BlueprintCallable, Category = "Levels")
		virtual void LoadDynamicLevelInstanceForAllPlayers(class UMapAsset* MapAsset, FVector Location = FVector::ZeroVector, FRotator Rotation = FRotator::ZeroRotator);
	/*event that is triggered when a client has notified the server it's done loading a level*/
	UFUNCTION()
		virtual void OnPlayerLevelInstanceLoaded(class APlayerController* Player);	
	/*event that is triggered when a client has notified the server it's done loading a level*/
	UFUNCTION(BlueprintImplementableEvent, Category = "Levels", meta = (DisplayName = "On Player Level Instance Loaded"))
		void BP_OnPlayerLevelInstanceLoaded(class APlayerController* Player);

	/*called when all players have loaded*/
	UFUNCTION()
		virtual void OnLevelInstanceLoadedForAllPlayers();

	UFUNCTION(BlueprintImplementableEvent, Category = "Levels", meta = (DisplayName = "On Level Instance Loaded for All Players"))
		void BP_OnLevelInstanceLoadedForAllPlayers();

	template<typename T>
	FORCEINLINE T* GetLevelScript(ULevel* LevelOwner)
	{
		static_assert(TPointerIsConvertibleFromTo<T, ALevelScriptActor>::Value,
			"T must be ALevelScriptActor or a subclass.");

		if (!LevelOwner)
		{
			const FString ClassName = T::StaticClass()->GetName();
			ensureMsgf(false, TEXT("GetLevelScript<%s>: Level is null."), *ClassName);
			return nullptr;
		}

		ALevelScriptActor* LSA = LevelOwner->GetLevelScriptActor();
		return Cast<T>(LSA);
	}
	
};
