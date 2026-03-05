// Fill out your copyright notice in the Description page of Project Settings.


#include "Subsystems/CharacterSubsystem.h"
#include "Characters/FirstPersonCharacter.h"
#include "Characters/CharacterNameAsset.h"
#include "Players/FirstPersonPlayerController.h"
#include "Players/FirstPersonPlayerState.h"
#include "GameModes/FirstPersonGame.h"
#include "GameModes/FirstPersonGameState.h"

void UCharacterSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	/*initialize the default Character Names asset*/
	static const FString NameAssetPath = TEXT("/FirstPersonModule/Blueprints/DataAssets/CharacterNames.CharacterNames");
	CharacterNames = Cast<UCharacterNameAsset>(StaticLoadObject(UCharacterNameAsset::StaticClass(), nullptr, *NameAssetPath));	
	
	Super::Initialize(Collection);
}

void UCharacterSubsystem::InitCharacters(TArray<class ACharacterInfo*> CharacterList)
{
	Characters.Empty();
	Characters = CharacterList;

	NotifyOfCharacterChange();
}

void UCharacterSubsystem::AddCharacter(ACharacterInfo* CharacterInfo)
{
	Characters.AddUnique(CharacterInfo);
	NotifyOfCharacterChange();	
}

void UCharacterSubsystem::AddCharacterFromAsset(UCharacterAsset* Character)
{
	if (!Character)
	{
		UE_LOG(LogTemp, Warning, TEXT("UCharacterSubsystem::AddCharacterFromAsset() - Invalid Character found"));
		return;
	}

	ACharacterInfo* NewCharacter = CreateCharacterInfoFromAsset(Character);
	AddCharacter(NewCharacter);
}

TArray<class ACharacterInfo*> UCharacterSubsystem::GetAllCharacters()
{
	return Characters;
}

TArray<class ACharacterInfo*> UCharacterSubsystem::GetUnselectedCharacters()
{
	TArray<ACharacterInfo*> UnselectedCharacters;

	for (ACharacterInfo* CharInfo : GetAllCharacters())
	{
		if (CharInfo->IsSelected() == false)
			UnselectedCharacters.AddUnique(CharInfo);
	}

	return UnselectedCharacters;	
}

ACharacterInfo* UCharacterSubsystem::CreateCharacterInfo(APlayerController* Owner)
{
	/*init*/
	ACharacterInfo* Character = nullptr;
	FActorSpawnParameters SpawnParams;	
	SpawnParams.Owner = Owner;

#if WITH_EDITOR
	SpawnParams.bHideFromSceneOutliner = true;
#endif

	/*spawn*/
	Character = GetWorld()->SpawnActor<ACharacterInfo>(SpawnParams);
	ECharacterGender Gender = (FMath::RandRange(0, 1) == 0) ? ECharacterGender::Male : ECharacterGender::Female;

	/*randomize name*/
	Character->FirstName = GetRandomFirstName(Gender);
	Character->LastName = GetRandomLastName();
	Character->Callsign = GetRandomCallsign();

	AddCharacter(Character);
	
	return Character;
}

ACharacterInfo* UCharacterSubsystem::CreateCharacterInfoFromAsset(UCharacterAsset* CharacterAsset, APlayerController* Owner)
{
	ACharacterInfo* Character = CreateCharacterInfo(Owner);

	if(Character)
		Character->InitFromAsset(CharacterAsset);

	return Character;
}

void UCharacterSubsystem::SetHoveredCharacter(ACharacterInfo* Character)
{
	if (Character != nullptr && HoveredCharacter != Character)
	{
		HoveredCharacter = Character;
	}
}

ACharacterInfo* UCharacterSubsystem::GetHoveredCharacter()
{
	return HoveredCharacter;
}

void UCharacterSubsystem::SetSelectedCharacter(ACharacterInfo* Character)
{
	if (Character != nullptr && Character != SelectedCharacter)
	{
		SelectedCharacter = Character;

		if (OnCharacterSelectionChanged.IsBound())
			OnCharacterSelectionChanged.Broadcast();
	}
}

void UCharacterSubsystem::UnselectCharacter(ACharacterInfo* Character)
{
	if (!Character)
		SelectedCharacter = nullptr;

	else if (SelectedCharacter == Character)
		SelectedCharacter = nullptr;
}

ACharacterInfo* UCharacterSubsystem::GetSelectedCharacter()
{
	return SelectedCharacter;
}

FString UCharacterSubsystem::GetRandomFirstName(ECharacterGender Gender)
{	
	if (Gender == ECharacterGender::Male)
	{
		if (CharacterNames && CharacterNames->MaleFirstNames.Num() > 0)
		{
			float randomIndex = FMath::RandRange(0, CharacterNames->MaleFirstNames.Num() - 1);
			return CharacterNames->MaleFirstNames[randomIndex];
		}

		else
			return "John";
		
	}
	else
	{
		if (CharacterNames && CharacterNames->FemaleFirstNames.Num() > 0)
		{
			float randomIndex = FMath::RandRange(0, CharacterNames->FemaleFirstNames.Num() - 1);
			return CharacterNames->FemaleFirstNames[randomIndex];
		}

		else
			return "Jane";		
	}
}

FString UCharacterSubsystem::GetRandomLastName()
{
	if (CharacterNames && CharacterNames->LastNames.Num() > 0)
	{
		float randomIndex = FMath::RandRange(0, CharacterNames->LastNames.Num() - 1);
		return CharacterNames->LastNames[randomIndex];
	}

	else
		return "Doe";
}

FString UCharacterSubsystem::GetRandomCallsign()
{
	if (CharacterNames && CharacterNames->Callsigns.Num() > 0)
	{
		float randomIndex = FMath::RandRange(0, CharacterNames->Callsigns.Num() - 1);
		return CharacterNames->Callsigns[randomIndex];
	}
	else
		return "Meat Gibs";
}


void UCharacterSubsystem::RegisterPlayerController(AFirstPersonPlayerController* Controller)
{
	PlayerControllers.AddUnique(Controller);
}

void UCharacterSubsystem::OnNotifiedOfPlayerDeath(AFirstPersonPlayerController* PlayerController, AFirstPersonCharacter* PlayerPawn, class AActor* DamageCauser, class AController* DamageInstigator, const UDamageType* DamageType)
{
	/*server check*/
	if (IsServer())
	{
		if (AFirstPersonGame* GM = GetWorld()->GetAuthGameMode<AFirstPersonGame>())
			GM->OnPlayerDeath(PlayerPawn, DamageCauser, DamageInstigator, DamageType);
	}


	if (OnPlayerDeath.IsBound())
		OnPlayerDeath.Broadcast(PlayerController, PlayerPawn);
}

TArray<AFirstPersonCharacter*> UCharacterSubsystem::GetAllPlayerPawns()
{
	TArray<AFirstPersonCharacter*> Pawns;
	for (AFirstPersonPlayerController* PC : PlayerControllers)
	{
		if (AFirstPersonCharacter* Pawn = Cast<AFirstPersonCharacter>(PC->GetPawn()))
			Pawns.Add(Pawn);
	}

	return Pawns;
}

//==========================================
//=================LOADOUTS=================
//==========================================

AInventoryLoadout* UCharacterSubsystem::GetCharacterLoadout()
{
	if (SelectedCharacter)
		return SelectedCharacter->Loadout;

	return nullptr;
}



//=========================================
//================UTILITIES================
//=========================================
void UCharacterSubsystem::NotifyOfCharacterChange()
{	
	if (IsServer() && GetWorld())
	{
		/*push update to clients the character changes*/
		if(AFirstPersonGameState* GS = GetWorld()->GetGameState<AFirstPersonGameState>())
			GS->SyncCharacterList();
	}

	if (OnCharacterListChanged.IsBound())
		OnCharacterListChanged.Broadcast();
}

bool UCharacterSubsystem::IsServer()
{
	if (!GetWorld())
		return false;

	return GetWorld()->GetNetMode() < NM_Client;
}


