// Fill out your copyright notice in the Description page of Project Settings.

#include "Subsystems/MapSubsystem.h"

/*asset handling*/
#include "Engine/StreamableManager.h"
#include "Engine/AssetManager.h"

/*gamemodes*/
#include "GameModes/FirstPersonGame.h"
#include "GameModes/FirstPersonGameState.h"
#include "GameModes/FirstPersonLevelScript.h"

/*maps*/
#include "Maps/MapAsset.h"

/*players*/
#include "Players/FirstPersonPlayerController.h"

/*utilities*/
#include "Kismet/GameplayStatics.h"
#include "../Logging.h"

void UMapSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);

	InitializeDefaultMaps();
}

void UMapSubsystem::Deinitialize()
{
	Super::Deinitialize();
}


/*InitializeDefaultMaps() - Gathers all the maps marked as "Map" inside the Asset Manager (under Project Settings) directories defined for the same type*/
void UMapSubsystem::InitializeDefaultMaps()
{
	/*gather all the missions and load them*/
	UAssetManager& AssetManager = UAssetManager::Get();

	FPrimaryAssetId MapAssetId("Map");
	TArray<FPrimaryAssetId> MapId;
	AssetManager.GetPrimaryAssetIdList(UMapAsset::PrimaryAssetType, MapId);

	Maps.Reset();

	// Load them all (sync)
	if (TSharedPtr<FStreamableHandle> Handle = AssetManager.LoadPrimaryAssets(MapId))
	{
		Handle->WaitUntilComplete();
	}

	// Now collect loaded objects
	for (const FPrimaryAssetId& Id : MapId)
	{
		if (UObject* Loaded = AssetManager.GetPrimaryAssetObject(Id))
		{
			if (UMapAsset* Map = Cast<UMapAsset>(Loaded))
			{
				if(Map->bExcludeFromSubsystem == false)
					Maps.Add(Map);
			}
		}
	}
}

void UMapSubsystem::GetMapsByClass(TSubclassOf<UMapAsset> MapClass, TArray<UMapAsset*>& OutMaps) const
{
	/*safety check*/
	if (!*MapClass)
		return;

	/*clear array*/
	OutMaps.Reset();

	/*add mapss*/
	for (UMapAsset* Map : Maps)
	{
		if (Map && Map->IsA(MapClass))
			OutMaps.Add(Map);
	}
}

void UMapSubsystem::SetCurrentMap(UMapAsset* MapAsset)
{
	/*safety check*/
	if (!MapAsset)
		return;

	if (IsServer())
	{
		PreviousMap = CurrentMap;
		CurrentMap = MapAsset;

		/*notify gamemode which will update gamestate which will in turn update client's MapSubsystem calling this same function back*/
		if (GetGameMode())
			GetGameMode()->OnCurrentMapUpdated(MapAsset);
	}
}

/*Client_CurrentMapUpdated() - Pseudo-Client function that is updated by the GameState notifying it when the server has updated the "Current Map" property*/
void UMapSubsystem::Client_OnCurrentMapUpdated(UMapAsset* MapAsset)
{
	PreviousMap = CurrentMap;
	CurrentMap = MapAsset;
}

const TArray<UMapAsset*> UMapSubsystem::GetMapRotation() const
{
	return MapRotation;
}

void UMapSubsystem::SetMapRotation(TArray<UMapAsset*> MapList)
{
	if (IsServer())
	{
		MapRotation.Empty(); // clear old list
		MapRotation = MapList;

		/*notify gamemode which will update gamestate which will in turn update client's MapSubsystem calling this same function back*/
		if (GetGameMode())
			GetGameMode()->OnMapRotationUpdated(MapList);
	}
}

void UMapSubsystem::AddToMapRotation(UMapAsset* MapAsset)
{
	if (!MapAsset)
		return;

	if (IsServer())
	{
		MapRotation.Add(MapAsset);

		/*notify gamemode/gamestate*/
		if (AFirstPersonGame* GM = GetWorld() ? GetWorld()->GetAuthGameMode<AFirstPersonGame>() : nullptr)
			GM->OnMapRotationUpdated(MapRotation);
	}
}

void UMapSubsystem::Client_OnMapRotationUpdated(TArray<UMapAsset*> MapList)
{
	MapRotation.Empty(); // clear old list
	MapRotation = MapList;
}

const UMapAsset* UMapSubsystem::GetNextMap() const
{
	return NextMap;
}

void UMapSubsystem::SetNextMap(UMapAsset* MapAsset)
{
	if (!MapAsset)
		return;

	if (IsServer())
	{
		NextMap = MapAsset;
		if (GetGameMode())
			GetGameMode()->OnNextMapUpdated(MapAsset);
	}

	/*clients*/
	else
	{
		/*Request a new map via the PlayerController so the server can evaluate if this player has permissions to do so*/
		if (AFirstPersonPlayerController* PC = Cast<AFirstPersonPlayerController>(UGameplayStatics::GetPlayerController(GetWorld(), 0)))
		{
			PC->ServerSelectNextMap(MapAsset);
		}
	}
}

void UMapSubsystem::ClearNextMap()
{
	if (IsServer())
	{
		NextMap = nullptr;
	}
}

void UMapSubsystem::Client_OnNextMapUpdated(UMapAsset* MapAsset)
{
	NextMap = MapAsset;
}

const UMapAsset* UMapSubsystem::GetHoveredMap() const
{
	return HoveredMap;
}

void UMapSubsystem::SetHoveredMap(UMapAsset* MapAsset)
{
	HoveredMap = MapAsset;
}

void UMapSubsystem::ClearHoveredMap()
{
	HoveredMap = nullptr;
}

bool UMapSubsystem::IsMapSelected(UMapAsset* MapInfo)
{
	if (CurrentMap == MapInfo)
		return true;
	else if (NextMap == MapInfo)
		return true;
	else if (MapRotation.Contains(MapInfo))
		return true;
	else
		return false;
}

FString UMapSubsystem::SantizeMapPath(FString Path)
{
	UE_LOG(LogMapSubsystem, Log, TEXT("UMapSubsystem::SantizeMapPath(%s)"), *Path);
	int32 DotIndex;

	if (Path.FindChar('.', DotIndex))
	{
		Path.LeftInline(DotIndex);
		UE_LOG(LogMapSubsystem, Log, TEXT("Santized Path : %s"), *Path);
		return Path;
	}
	return Path;
}

const UMapAsset* UMapSubsystem::GetCurrentMap() const
{
	return CurrentMap;
}

AFirstPersonGame* UMapSubsystem::GetGameMode()
{
	return GetWorld() ? GetWorld()->GetAuthGameMode<AFirstPersonGame>() : nullptr;
}

AFirstPersonGameState* UMapSubsystem::GetGameState()
{
	return GetWorld() ? GetWorld()->GetGameState<AFirstPersonGameState>() : nullptr;
}
