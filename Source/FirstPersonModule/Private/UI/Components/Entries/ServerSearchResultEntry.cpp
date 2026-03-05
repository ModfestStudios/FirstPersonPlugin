// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/Components/Entries/ServerSearchResultEntry.h"
#include "Subsystems/ServerSubsystem.h"

/*engine*/
#include "Engine/GameInstance.h"

void UServerSearchResultEntry::NativeOnListItemObjectSet(UObject* ListItemObject)
{
	SearchResult = Cast<UServerBrowserSearchResult>(ListItemObject);

	/*call BP implementation*/
	Execute_OnListItemObjectSet(Cast<UObject>(this), ListItemObject);
}

void UServerSearchResultEntry::NativeOnItemSelectionChanged(bool bIsSelected)
{
	UServerSubsystem* ServerSubsystem = GetGameInstance()->GetSubsystem<UServerSubsystem>();

	/*select*/
	if (bIsSelected)
	{
		if (ServerSubsystem)
		{
			ServerSubsystem->OnServerSelected(SearchResult);
		}
	}
	/*unselect*/
	else
	{
		if (ServerSubsystem && ServerSubsystem->GetSelectedServer() == SearchResult)
			ServerSubsystem->ClearServerSelection();
	}

	Execute_BP_OnItemSelectionChanged(Cast<UObject>(this), bIsSelected);
}
