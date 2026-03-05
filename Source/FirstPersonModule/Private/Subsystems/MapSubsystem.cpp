// Fill out your copyright notice in the Description page of Project Settings.

#include "Subsystems/MapSubsystem.h"

/*asset handling*/
#include "Engine/StreamableManager.h"
#include "Engine/AssetManager.h"

/*maps*/
#include "Maps/MapAsset.h"

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