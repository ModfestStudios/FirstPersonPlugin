// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/Components/Entries/CharacterInfoEntry.h"
#include "Characters/CharacterInfo.h"

/*subsystems*/
#include "Subsystems/CharacterSubsystem.h"
#include "Subsystems/MissionSubsystem.h"

/*engine*/
#include "Engine/GameInstance.h"

void UCharacterInfoEntry::NativeOnListItemObjectSet(UObject* ListItemObject)
{
	if (ACharacterInfo* CharInfo = Cast<ACharacterInfo>(ListItemObject))
		SetCharacterInfo(CharInfo);	

	/*call to blueprint version*/
	Execute_OnListItemObjectSet(Cast<UObject>(this),ListItemObject);
}

void UCharacterInfoEntry::NativeOnItemSelectionChanged(bool bIsSelected)
{
	if (!bSelectable)
	{
		Execute_BP_OnItemSelectionChanged(Cast<UObject>(this), bIsSelected);
		return;
	}	

	if (bIsSelected)
		SelectCharacter();
	else
		UnselectCharacter();
}

void UCharacterInfoEntry::SetCharacterInfo(ACharacterInfo* NewCharacterInfo)
{
	CharacterInfo = NewCharacterInfo;

	//OnRosterInfo();

	if (OnCharacterInfoChanged.IsBound())
		OnCharacterInfoChanged.Broadcast();
}

bool UCharacterInfoEntry::IsCharacterSelected()
{
	if (bSelectable)
	{
		if (GetCharacterSubsystem() && GetCharacterSubsystem()->GetSelectedCharacter() == CharacterInfo)
			return true;
	}

	return false;
}

void UCharacterInfoEntry::SelectCharacter()
{
	
	if (!bSelectable || !CharacterInfo || !GetGameInstance())
		return;

	
	if (UCharacterSubsystem* CSS = GetCharacterSubsystem())
	{
		CSS->SetSelectedCharacter(CharacterInfo);
	}	
}

void UCharacterInfoEntry::UnselectCharacter()
{
	if (!bSelectable || !CharacterInfo || !GetGameInstance())
		return;

	
	if (GetCharacterSubsystem())
	{
		GetCharacterSubsystem()->UnselectCharacter(CharacterInfo);
	}
}

UCharacterSubsystem* UCharacterInfoEntry::GetCharacterSubsystem()
{
	if(CharacterSubsystem == nullptr)
		CharacterSubsystem = GetGameInstance()->GetSubsystem<UCharacterSubsystem>();

	return CharacterSubsystem;
}

UMissionSubsystem* UCharacterInfoEntry::GetMissionSubsystem()
{
	if (MissionSubsystem == nullptr)
		MissionSubsystem = GetGameInstance()->GetSubsystem<UMissionSubsystem>();

	return MissionSubsystem;
}
