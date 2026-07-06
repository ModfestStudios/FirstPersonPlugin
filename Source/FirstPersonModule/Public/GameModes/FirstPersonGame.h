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

	/*if enabled - will show the lobby widget when players join*/
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Lobby")
		bool bShowLobbyOnJoin = false;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Classes")
		TSubclassOf<class UGameLobbyWidget> LobbyWidgetClass;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Classes")
		TSubclassOf<UUserWidget> EscapeMenuWidgetClass;

	/*widget for displaying Server Info to Clients*/
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Classes")
		TSubclassOf<UUserWidget> ServerInfoWidgetClass;

	/*class used to store character info per-pawn/player*/
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Classes")
		TSubclassOf<class ACharacterInfo> CharacterInfoClass;


	UPROPERTY(EditAnywhere, Category = "Spectators")
	bool bStartPlayersWithSpectatorPawn = true;
	UPROPERTY(EditAnywhere, Category = "Spectators")
		bool bUseSpectatorSpawns = true;
	UPROPERTY(VisibleAnywhere, Category = "Spectators")
		FName SpectatorSpawnTag = FName("SpectatorSpawn");

	UPROPERTY()
		TArray<class AActor*> SpectatorSpawnLocations;
	

	/*when enabled - character death is logged and saved*/
	UPROPERTY(EditDefaultsOnly, Category = "Characters|Spawning")
		bool bPermanentDeath = false;



	//=============================================================================================================================
	//==========================================================FUNCTIONS==========================================================
	//=============================================================================================================================
	AFirstPersonGame();


	//========================
	//=======INITIATION=======
	//========================
	
	/**
	 * Initialize the game.
	 * The GameMode's InitGame() event is called before any other functions (including PreInitializeComponents() )
	 * and is used by the GameMode to initialize parameters and spawn its helper classes.
	 * @warning: this is called before actors' PreInitializeComponents.
	 */
	virtual void InitGame(const FString& MapName, const FString& Options, FString& ErrorMessage) override;
	/**
	 * Initialize the GameState actor with default settings
	 * called during PreInitializeComponents() of the GameMode after a GameState has been spawned
	 * as well as during Reset()
	 */
	virtual void InitGameState() override;



	//=========================
	//=========PLAYERS=========
	//=========================


protected:


	/**
	 * Accept or reject a player attempting to join the server.  Fails login if you set the ErrorMessage to a non-empty string.
	 * PreLogin is called before Login.  Significant game time may pass before Login is called
	 *
	 * @param	Options					The URL options (e.g. name/spectator) the player has passed
	 * @param	Address					The network address of the player
	 * @param	UniqueId				The unique id the player has passed to the server
	 * @param	ErrorMessage			When set to a non-empty value, the player will be rejected using the error message set
	 */
	virtual void PreLogin(const FString& Options, const FString& Address, const FUniqueNetIdRepl& UniqueId, FString& ErrorMessage) override;
	/**
	 * Called to login new players by creating a player controller, overridable by the game
	 *
	 * Sets up basic properties of the player (name, unique id, registers with backend, etc) and should not be used to do
	 * more complicated game logic.  The player controller is not fully initialized within this function as far as networking is concerned.
	 * Save "game logic" for PostLogin which is called shortly afterward.
	 *
	 * @param NewPlayer pointer to the UPlayer object that represents this player (either local or remote)
	 * @param RemoteRole the remote role this controller has
	 * @param Portal desired portal location specified by the client
	 * @param Options game options passed in by the client at login
	 * @param UniqueId platform specific unique identifier for the logging in player
	 * @param ErrorMessage [out] error message, if any, why this login will be failing
	 *
	 * If login is successful, returns a new PlayerController to associate with this player. Login fails if ErrorMessage string is set.
	 *
	 * @return a new player controller for the logged in player, NULL if login failed for any reason
	 */
	virtual APlayerController* Login(UPlayer* NewPlayer, ENetRole InRemoteRole, const FString& Portal, const FString& Options, const FUniqueNetIdRepl& UniqueId, FString& ErrorMessage) override;

	/** Called after a successful login.  This is the first place it is safe to call replicated functions on the PlayerController. */
	virtual void PostLogin(APlayerController* NewPlayer) override;

	/** Called when a Controller with a PlayerState leaves the game or is destroyed */
	virtual void Logout(AController* Exiting) override;
	
	virtual void HandleStartingNewPlayer_Implementation(APlayerController* NewPlayer) override;
	virtual void RestartPlayer(AController* NewPlayer) override;


public:
	UFUNCTION()
		virtual void OnPlayerReadyStateChanged(class AFirstPersonPlayerController* Player, bool bReady);
	UFUNCTION()
		virtual void OnAllPlayersReadied();
	UFUNCTION(BlueprintImplementableEvent, Category = "Players", meta = (DisplayName = "On All Players Readied"))
		void BP_OnAllPlayersReadied();
	UFUNCTION(BlueprintPure, Category = "Players")
		bool AllPlayersReady();
protected:
	UFUNCTION(BlueprintImplementableEvent, Category = "Players", meta = (DisplayName = "On Player Ready State Changed"))
		void BP_OnPlayerReadyStateChanged(class AFirstPersonPlayerController* Player, bool bReady);

	




public:
	virtual int32 GetNumPlayers() override;
	virtual int32 GetNumSpectators() override;
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


	//==========================
	//========SPECTATORS========
	//==========================
	public:
		UFUNCTION()
			void InitSpectatorLocations();
		UFUNCTION(BlueprintCallable, Category = "Spectators")
			AActor* FindSpectatorSpawnLocation(class AFirstPersonPlayerController* Player);
		UFUNCTION(BlueprintCallable, Category = "Spectators")
		virtual void MoveSpectatorToLocation(class AFirstPersonPlayerController* Player, AActor* LocationRef);

		virtual void SpawnAndPossessSpectatorPawn(AFirstPersonPlayerController* Player);




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
