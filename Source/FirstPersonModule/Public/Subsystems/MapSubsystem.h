// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "MapSubsystem.generated.h"

/**
 * 
 */
UCLASS()
class FIRSTPERSONMODULE_API UMapSubsystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()
public:


	/*all available maps*/
	UPROPERTY()
		TArray<class UMapAsset*> Maps;

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
	/*list of maps queued in order (if using a map rotation style setup)*/
	UPROPERTY()
		TArray<UMapAsset*> MapRotation;

	/*this is used in UI widgets that want to display information based on a widget that's hovered over (automatically set by UMapInfoEntry*/
	UPROPERTY()
		UMapAsset* HoveredMap;


	//=========================================================================================================================================
	//================================================================FUNCTIONS================================================================
	//=========================================================================================================================================
public:
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;
	virtual void Deinitialize() override;



	//======================
	//=========MAPS=========
	//======================
public:
	UFUNCTION()
		void InitializeDefaultMaps();
	UFUNCTION(BlueprintCallable, Category = "Server Subsystem|Maps", meta = (DeterminesOutputType="MapClass", DynamicOutputParam="OutMaps"))
		void GetMapsByClass(TSubclassOf<UMapAsset> MapClass, TArray<UMapAsset*>& OutMaps) const;

	//===================================
	//===========MAP ROTATIONS===========
	//===================================
public:
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


	//=======================
	//=======UTILITIES=======
	//=======================
public:
	bool IsServer() { return GetWorld() && GetWorld()->GetNetMode() < NM_Client; }
	class AFirstPersonGame* GetGameMode();
	class AFirstPersonGameState* GetGameState();
};
