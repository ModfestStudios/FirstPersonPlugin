// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Info.h"
#include "Inventory/Inventory.h"
#include "CharacterInfo.generated.h"

UENUM(BlueprintType)
enum class ECharacterGender : uint8
{
	Male,
	Female
};

/**
 * 
 */
UCLASS(NotBlueprintable)
class FIRSTPERSONMODULE_API ACharacterInfo : public AInfo
{
	GENERATED_BODY()
public:

	
	UPROPERTY(Replicated, BlueprintReadOnly, Category = "Character", meta = (ExposeOnSpawn = "TRUE"))
		FString FirstName;	
	UPROPERTY(Replicated,BlueprintReadOnly, Category = "Character", meta = (ExposeOnSpawn = "TRUE"))
		FString LastName;
	UPROPERTY(Replicated, BlueprintReadOnly, Category = "Character", meta = (ExposeOnSpawn = "TRUE"))
		FString Callsign;
	
	UPROPERTY(Replicated, BlueprintReadOnly, Category = "Character", meta = (ExposeOnSpawn = "TRUE"))
		ECharacterGender Gender = ECharacterGender::Male;


	/*the roster we're assigned to*/
	UPROPERTY(BlueprintReadOnly, Category = "Character|Roster")
		class ARosterInfo* AssignedRoster;

	UPROPERTY(BlueprintReadOnly, Category = "Character|Inventory")
		class AInventoryLoadout* Loadout;
	


	//===============================================================================================
	//===========================================FUNCTIONS===========================================
	//===============================================================================================
public:
	ACharacterInfo();

	virtual void BeginPlay() override;
	
	UFUNCTION(BlueprintCallable, Category = "Character")
		virtual void InitFromAsset(class UCharacterAsset* Asset);
	UFUNCTION(BlueprintCallable, Category = "Character")
		virtual void RandomizeName();

	UFUNCTION(BlueprintPure, Category = "Character", meta=(DisplayName="Get Lastname, Firstname"))
		FString GetLastNameFirstName();


	UFUNCTION()
		virtual bool IsSelected();
	UFUNCTION()
		virtual bool IsAssignedToRoster();
	UFUNCTION()
		virtual void SetAssignedRoster(class ARosterInfo* Roster);
	UFUNCTION()
		virtual void ClearAssignedRoster();

};
