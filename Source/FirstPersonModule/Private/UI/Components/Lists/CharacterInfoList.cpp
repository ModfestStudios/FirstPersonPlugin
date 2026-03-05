// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/Components/Lists/CharacterInfoList.h"

/*subsystems*/
#include "Subsystems/CharacterSubsystem.h"
#include "Subsystems/MissionSubsystem.h"

/*engine*/
#include "Engine/GameInstance.h"


void UCharacterInfoList::NativeOnInitialized()
{
	/*bind to character subsystem changes*/	
	if (UCharacterSubsystem* CSS = GetGameInstance()->GetSubsystem<UCharacterSubsystem>())
	{
		CSS->OnCharacterListChanged.AddDynamic(this, &UCharacterInfoList::OnCharacterListChanged);
	}

	/*bind to MissionSubsystem Team Roster changes*/
	if (UMissionSubsystem* MSS = GetGameInstance()->GetSubsystem<UMissionSubsystem>())
	{
		MSS->OnTeamRosterChanged.AddDynamic(this, &UCharacterInfoList::OnCharacterListChanged);
	}

	RefreshCharacterList();
	
	Super::NativeOnInitialized();
	
}

void UCharacterInfoList::NativeConstruct()
{
	Super::NativeConstruct();


	if (LIST_CharacterList)
	{
		LIST_CharacterList->OnItemSelectionChanged().AddUObject(this, &UCharacterInfoList::OnSelectionChanged); //bind change events to custom function
	}
}

void UCharacterInfoList::OnSelectionChanged(UObject* Item)
{
	if (!LIST_CharacterList)
		return;


	//bool bIsSelected = LIST_CharacterList->IsItemSelected(Item);
	//ACharacterInfo* Character = Cast<ACharacterInfo>(Item);
}

void UCharacterInfoList::OnCharacterListChanged()
{
	RefreshCharacterList();
}

void UCharacterInfoList::RefreshCharacterList()
{
	Characters.Empty();
	LIST_CharacterList->ClearListItems();

	if (CharacterSelectionType == ECharacterInfoListType::UnassignedTeamMembers)
	{
		if (UMissionSubsystem* MSS = GetGameInstance()->GetSubsystem<UMissionSubsystem>())
		{
			Characters = MSS->GetUnassignedTeamMembers();
		}
	}
	else if (CharacterSelectionType == ECharacterInfoListType::AssignedTeamMembers)
	{
		if (UMissionSubsystem* MSS = GetGameInstance()->GetSubsystem<UMissionSubsystem>())
		{
			Characters = MSS->GetAssignedTeamMembers();
		}
	}

	else
	{
		if (UCharacterSubsystem* CSS = GetGameInstance()->GetSubsystem<UCharacterSubsystem>())
		{
			if (CharacterSelectionType == ECharacterInfoListType::AllCharacters)
				Characters = CSS->GetAllCharacters();
			else if (CharacterSelectionType == ECharacterInfoListType::UnselectedCharacters)
				Characters = CSS->GetUnselectedCharacters();
			
		}
	}	

	/*update UMG widget list*/
	if (LIST_CharacterList)
	{
		for (ACharacterInfo* Character : Characters)
			LIST_CharacterList->AddItem(Character);
	}	

	BP_OnRefreshCharacterList();
}
