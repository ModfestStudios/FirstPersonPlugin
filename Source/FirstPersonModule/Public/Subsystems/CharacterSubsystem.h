// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "Characters/CharacterInfo.h"
#include "Characters/CharacterAsset.h"
#include "CharacterSubsystem.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnCharacterListChanged);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnCharacterSelectionChanged);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnPlayerDeath, class AFirstPersonPlayerController*, PlayerController, class AFirstPersonCharacter*, PlayerPawn);


/**
 * 
 */
UCLASS(meta = (DisplayName = "Character Subsystem"))
class FIRSTPERSONMODULE_API UCharacterSubsystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()
public:


	/*character list*/
private:
	UPROPERTY()
		TArray<class ACharacterInfo*> Characters;


	UPROPERTY()
		class ACharacterInfo* SelectedCharacter;
	UPROPERTY()
		class ACharacterInfo* HoveredCharacter;


	UPROPERTY()
		class UCharacterNameAsset* CharacterNames;

	/*PLAYERS*/
	UPROPERTY()
		TArray<class AFirstPersonPlayerController*> PlayerControllers;



	/*EVENTS*/
public:

	/*called whenever the user "selects" a new character*/
	UPROPERTY(BlueprintAssignable, Category = "Characters")
		FOnCharacterSelectionChanged OnCharacterSelectionChanged;
	/*called anytime a character has been added/removed*/
	UPROPERTY(BlueprintAssignable, Category = "Characters")
		FOnCharacterListChanged OnCharacterListChanged;
	UPROPERTY(BlueprintAssignable, Category = "Players")
		FOnPlayerDeath OnPlayerDeath;


	//=====================================================================================================
	//==============================================FUNCTIONS==============================================
	//=====================================================================================================
public:
	virtual void Initialize(FSubsystemCollectionBase& Collection);


	/*clears old list and inserts entire list of playable characters*/
	UFUNCTION(BlueprintCallable, Category = "Characters|Character Management")
		virtual void InitCharacters(TArray<class ACharacterInfo*> CharacterList);
	/*adds a character to possible playable characters*/
	UFUNCTION(BlueprintCallable, Category = "Characters|Character Management")
		virtual void AddCharacter(class ACharacterInfo* CharacterInfo);
	UFUNCTION(BlueprintCallable, Category = "Characters|Character Management")
		virtual void AddCharacterFromAsset(class UCharacterAsset* Character);
	UFUNCTION(BlueprintCallable, Category = "Characters|Character Management")
		virtual TArray<class ACharacterInfo*> GetAllCharacters();
	UFUNCTION(BlueprintCallable, Category = "Characters|Character Management")
		virtual TArray<class ACharacterInfo*> GetUnselectedCharacters();

	UFUNCTION(BlueprintCallable, Category = "Characters")
		class ACharacterInfo* CreateCharacterInfo(APlayerController* Owner = nullptr);
	UFUNCTION(BlueprintCallable, Category = "Characters")
		class ACharacterInfo* CreateCharacterInfoFromAsset(class UCharacterAsset* CharacterAsset, APlayerController* Owner = nullptr);	



	/*character selection*/
	UFUNCTION(BlueprintCallable, Category = "Characters")
		virtual void SetHoveredCharacter(class ACharacterInfo* Character);
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Characters")
		class ACharacterInfo* GetHoveredCharacter();

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Characters")
		class ACharacterInfo* GetSelectedCharacter();

	UFUNCTION(BlueprintCallable, Category = "Characters")
		virtual void SetSelectedCharacter(class ACharacterInfo* Character);
	
	UFUNCTION(BlueprintCallable, Category = "Characters")
		virtual void UnselectCharacter(class ACharacterInfo* Character);
		




	UFUNCTION(BlueprintCallable, Category = "Characters")
		FString GetRandomFirstName(ECharacterGender Gender);
	UFUNCTION(BlueprintCallable, Category = "Characters")
		FString GetRandomLastName();
	UFUNCTION(BlueprintCallable, Category = "Characters")
		FString GetRandomCallsign();


public:
	UFUNCTION()
		virtual void RegisterPlayerController(class AFirstPersonPlayerController* Controller);
	/* informs the GameMaster of a player death - which can then broadcast it to everyone listening */
	UFUNCTION(BlueprintCallable, Category = "Players")
		virtual void OnNotifiedOfPlayerDeath(class AFirstPersonPlayerController* PlayerController, class AFirstPersonCharacter* PlayerPawn, class AActor* DamageCauser, class AController* DamageInstigator, const class UDamageType* DamageType);
	UFUNCTION(BlueprintCallable, Category = "Players")
		virtual TArray<class AFirstPersonCharacter*> GetAllPlayerPawns();


	//==========================================
	//=================LOADOUTS=================
	//==========================================
public:
	UFUNCTION(BlueprintCallable, Category = "Characters|Loadout")
		virtual class AInventoryLoadout* GetCharacterLoadout();


	//=========================================
	//================UTILITIES================
	//=========================================
protected:
	void NotifyOfCharacterChange();



	virtual bool IsServer();
	
};
