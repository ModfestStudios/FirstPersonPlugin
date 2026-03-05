// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Maps/MapAsset.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "Interfaces/OnlineSessionInterface.h"
#include "OnlineSessionSettings.h"
#include "Engine/World.h"
#include "ServerSubsystem.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnServerBrowserUpdate);
/**
 * 
 */

USTRUCT(BlueprintType)
struct FSessionInfo
{
	GENERATED_BODY()
public:

	UPROPERTY(BlueprintReadOnly, Category = "Session Info")
		FString ServerName;
	UPROPERTY(BlueprintReadOnly, Category = "Session Info")
		FString SessionType;
	UPROPERTY(BlueprintReadOnly, Category = "Session Info")
		int32 OpenConnections;
	UPROPERTY(BlueprintReadOnly, Category = "Session Info")
		int32 MaxConnections;
	UPROPERTY(BlueprintReadOnly, Category = "Session Info")
		bool bIsAdvertising;
	UPROPERTY(BlueprintReadOnly, Category = "Session Info")
		FString ServerStatus;
};

UCLASS()
class FIRSTPERSONMODULE_API UServerSubsystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()
public:

	
	UPROPERTY(BlueprintReadWrite, Category = "Server Subsystem|Server Settings")
		FString ServerName = "A Really Cool Server";
	UPROPERTY(BlueprintReadWrite, Category = "Server Subsystem|Server Settings")
		FString  Password;



	/*server browsing*/
	UPROPERTY(BlueprintReadOnly, Category = "Server Subsystem|Browsing")
		bool bSearchingForServers = false;
	UPROPERTY(BlueprintReadOnly, Category = "Serer Subsystem|Browsing")
		TArray<class UServerBrowserSearchResult*> SearchResults;
protected:
	UPROPERTY()
		class UServerBrowserSearchResult* ServerToJoin;
public:
	UPROPERTY(BlueprintReadWrite, Category = "Server Subsystem|Direct Connect")
		FString DirectConnectURL;



	/*map settings*/
private:	

	/*the current level we're on*/
	UPROPERTY()
		UMapAsset* CurrentMap;
	UPROPERTY()
		UMapAsset* PreviousMap;
	/*the next map queued up*/
	UPROPERTY()
		UMapAsset* NextMap;
	/*lis of maps queued in order (if using a map rotation style setup)*/
	UPROPERTY()
		TArray<UMapAsset*> MapRotation;

	/*this is used in UI widgets that want to display information based on a widget that's hovered over (automatically set by UMapInfoEntry*/
	UPROPERTY()
		UMapAsset* HoveredMap;



	/*session details*/
private:
	IOnlineSessionPtr SessionInterface;
	TSharedPtr<FOnlineSessionSearch> SessionSearch;
	FOnlineSessionSettings SessionSettings;

	/*events*/
public:
	UPROPERTY(BlueprintAssignable)
		FOnServerBrowserUpdate OnSearchResultsUpdated;

public:
	/*session consts*/
	static const FName SERVER_NAME;
	static const FName GAME_VERSION;
	static const FName GAME_MODE;

	//=========================================================================================================================
	//========================================================FUNCTIONS========================================================
	//=========================================================================================================================
public:

	virtual void Initialize(FSubsystemCollectionBase& Collection) override;
	virtual void Deinitialize() override;





	//===================================
	//===========MAP ROTATIONS===========
	//===================================

	UFUNCTION(BlueprintCallable, Category = "Server Subsystem|Maps")
		void SetCurrentMap(UMapAsset* MapAsset);
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Server Subsystem|Maps")
		const UMapAsset* GetCurrentMap() const;
	UFUNCTION()
		void Client_OnCurrentMapUpdated(UMapAsset* MapAsset);

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Server Subsystem|Maps")
		const TArray<UMapAsset*> GetMapRotation() const;
	UFUNCTION(BlueprintCallable, Category = "Server Subsystem|Maps")
		void SetMapRotation(TArray<UMapAsset*> MapList);
	UFUNCTION(BlueprintCallable, Category = "Server Subsystem|Maps")
		void AddToMapRotation(UMapAsset* MapAsset);
	UFUNCTION()
		void Client_OnMapRotationUpdated(TArray<UMapAsset*> MapList);
	
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Server Subsystem|Maps")
		const UMapAsset* GetNextMap() const;	
	UFUNCTION(BlueprintCallable, Category = "Server Subsystem|Maps")
		void SetNextMap(UMapAsset* MapAsset);
	UFUNCTION(BlueprintCallable, Category = "Server Subsystem|Maps")
		void ClearNextMap();
	UFUNCTION()
		void Client_OnNextMapUpdated(UMapAsset* MapAsset);

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Server Subsystem|Maps")
		const UMapAsset* GetHoveredMap() const;
	UFUNCTION(BlueprintCallable, Category = "Server Subsystem|Maps")
		void SetHoveredMap(UMapAsset* MapAsset);
	UFUNCTION(BlueprintCallable, Category = "Server Subsystem|Maps")
		void ClearHoveredMap();


	/*returns true if is current map, next map, or part of map rotation*/
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Server Subsystem|Maps")
		bool IsMapSelected(UMapAsset* MapInfo);
	/*will take a Map URL/Path and return a santized version*/
	UFUNCTION(BlueprintCallable, Category = "Server Subsystem|Maps")
		FString SantizeMapPath(FString Path);




	//======================
	//=======SESSIONS=======
	//======================
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Server Subsystem|Sessions")
		FSessionInfo GetCurrentSessionInfo();
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Server Subsystem|Sessions")
		bool IsSessionValid();
	UFUNCTION(BlueprintCallable, Category = "Server Subsystem|Sessions|LAN")
		void HostLANSession();


	//===================================
	//========JOINING & SEARCHING========
	//===================================
public:
	/*returns whether or not we're actively searching for servers*/
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Server Subsystem|Sessions")
		bool IsSearchingForServers();
	/*returns the amount of servers we've found in our search*/
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Server Subsystem|Sessions")
		int32 GetNumOfServersFound();
	UFUNCTION(BlueprintCallable, Category = "Server Subsystem|Sessions|LAN")
		void FindLANSessions();
//	UFUNCTION(BlueprintCallable, Category = "Server Subsystem|Sessions|LAN")
	UFUNCTION(BlueprintCallable, Category = "Server Subsystem|Sessions|LAN")
		void JoinLANSession();
		void JoinSession(const FOnlineSessionSearchResult& Result);
		void JoinDirectConnect(FString ServerURL);

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Server Subsystem|Sessions")
		bool HasSelectedServerToJoin();
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Server Subsystem|Sessions")
		class UServerBrowserSearchResult* GetSelectedServer();
	UFUNCTION(BlueprintCallable, Category = "Server Subsystem|Sessions")
		void OnServerSelected(UServerBrowserSearchResult* SearchResult);
	UFUNCTION(BlueprintCallable, Category = "Server Subsystem|Sessions")
		void ClearServerSelection();

	UFUNCTION()
		void ClearServerBrowserResults();

	void OnCreateSessionComplete(FName SessionName, bool bWasSuccessful);
	void OnFindSessionsComplete(bool bWasSuccessful);
	void OnJoinSessionComplete(FName SessionName, EOnJoinSessionCompleteResult::Type Result);


	//===========================
	//=========UTILITIES=========
	//===========================
	bool IsServer() { return GetWorld() && GetWorld()->GetNetMode() < NM_Client; }
	class AFirstPersonGame* GetGameMode();
	class AFirstPersonGameState* GetGameState();
	
};
