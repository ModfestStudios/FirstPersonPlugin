// Fill out your copyright notice in the Description page of Project Settings.


#include "Subsystems/ModSubsystem.h"
#include "Maps/MapAsset.h"

void UModSubsystem::AddMap(UMapAsset* NewMap)
{
	Maps.AddUnique(NewMap);
}

TArray<UMapAsset*> UModSubsystem::GetAllMaps()
{
	return Maps;
}

TArray<UMapAsset*> UModSubsystem::GetMapsByCategory(const FName Category)
{
	TArray<UMapAsset*> MapList;
	
	for (UMapAsset* Map : Maps)
	{
		if (Map->Category == Category)
			MapList.Add(Map);
	}	
	
	SortMapListAlphabetically(MapList);

	return MapList;;
}

TArray<UMapAsset*> UModSubsystem::GetMapsByTag(const FName Tag, const FName Value)
{
	TArray<UMapAsset*> MapList;

	for (UMapAsset* Map : Maps)
	{
		if (const FName* TagValue = Map->Tags.Find(Tag))
		{
			if(*TagValue == Value)
				MapList.Add(Map);
		}
			
	}

	SortMapListAlphabetically(MapList);

	return MapList;
}

void UModSubsystem::SortMapListAlphabetically(TArray<UMapAsset*>& MapList)
{
	MapList.Sort(UMapAsset::SortAlphabetically);
}
