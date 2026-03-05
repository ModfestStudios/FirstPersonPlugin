// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/Components/Entries/MapInfoEntry.h"
#include "Maps/MapAsset.h"
#include "Subsystems/ServerSubsystem.h"
#include "Blueprint/IUserObjectListEntry.h"
#include "Blueprint/IUserListEntry.h"

/*engine*/
#include "Engine/GameInstance.h"

void UMapInfoEntry::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);

	if (IsHovered() && bHoverState == false)
		NotifyOfHover();
	else if (!IsHovered() && bHoverState == true)
		NotifyOfUnhover();
}

void UMapInfoEntry::NativeOnListItemObjectSet(UObject* ListItemObject)
{
	if (UMapAsset* Map = Cast<UMapAsset>(ListItemObject))
		MapInfo = Map;

	Execute_OnListItemObjectSet(Cast<UObject>(this), ListItemObject);
	//NativeOnListItemObjectSet(ListItemObject);
}

void UMapInfoEntry::NativeOnItemSelectionChanged(bool bIsSelected)
{	
	if (bIsSelected)
	{
		if (bAddToMapRotationOnSelection)
			AddToMapRotation();
		else
			SetAsNextMap();
	}		
	
	Execute_BP_OnItemSelectionChanged(Cast<UObject>(this), bIsSelected);
}

void UMapInfoEntry::SetAsNextMap()
{
	if (!MapInfo)
		return;

	if (UServerSubsystem* ServerSubsystem = GetGameInstance()->GetSubsystem<UServerSubsystem>())
		ServerSubsystem->SetNextMap(MapInfo);
}

void UMapInfoEntry::AddToMapRotation()
{
	if (!MapInfo)
		return;

	if (UServerSubsystem* ServerSubsystem = GetGameInstance()->GetSubsystem<UServerSubsystem>())
		ServerSubsystem->AddToMapRotation(MapInfo);
}

void UMapInfoEntry::NotifyOfHover()
{
	bHoverState = true;


	if (UServerSubsystem* ServerSubsystem = GetGameInstance()->GetSubsystem<UServerSubsystem>())
	{
		if (ServerSubsystem->GetHoveredMap() != MapInfo)
			ServerSubsystem->SetHoveredMap(MapInfo);
	}

	/*call blueprint version*/
	BP_OnHover();

	/*notify anyone listening*/
	if (OnHover.IsBound())
		OnHover.Broadcast();
}


void UMapInfoEntry::NotifyOfUnhover()
{
	bHoverState = false;

	if (UServerSubsystem* ServerSubsystem = GetGameInstance()->GetSubsystem<UServerSubsystem>())
	{
		if (ServerSubsystem->GetHoveredMap() == MapInfo)
			ServerSubsystem->ClearHoveredMap();
	}

	/*call blueprint version*/
	BP_OnUnhover();

	/*notify anyone listening*/
	if (OnUnhover.IsBound())
		OnUnhover.Broadcast();
}

