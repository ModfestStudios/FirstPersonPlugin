// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Blueprint/IUserObjectListEntry.h"
#include "CharacterInfoEntry.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnCharacterInfoChanged);

/**
 * 
 */
UCLASS(abstract)
class FIRSTPERSONMODULE_API UCharacterInfoEntry : public UUserWidget, public IUserObjectListEntry
{
	GENERATED_BODY()
protected:

	UPROPERTY(EditAnywhere, Category = "Character|Selection")
		bool bSelectable = true;

	UPROPERTY(BlueprintReadOnly, Category = "Character")
		class ACharacterInfo* CharacterInfo;

	/*caching*/
	UPROPERTY()
		class UCharacterSubsystem* CharacterSubsystem;
	UPROPERTY()
		class UMissionSubsystem* MissionSubsystem;


	/*events*/
	UPROPERTY(BlueprintAssignable, Category = "Character")
		FOnCharacterInfoChanged OnCharacterInfoChanged;

	//==============================================================================================================
	//===================================================FUNCITON===================================================
	//==============================================================================================================
public:
	
	/*integration support for List UMG Widgets that automatically calls SetCharacterInfo() for us*/
	virtual void NativeOnListItemObjectSet(UObject* ListItemObject) override;
	virtual void NativeOnItemSelectionChanged(bool bIsSelected) override;


	/*call htis when wanting to set the CharacterInfo - will also call the events tagged for this*/
	UFUNCTION(BlueprintCallable, Category = "Character")
		virtual void SetCharacterInfo(class ACharacterInfo* NewCharacterInfo);

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Character")
		bool IsCharacterSelected();
	/*will actively select this character*/
	UFUNCTION(BlueprintCallable, Category = "Characters")
		virtual void SelectCharacter();
	UFUNCTION(BlueprintCallable, Category = "Characters")
		virtual void UnselectCharacter();
		

	//========UTILITIES========

	UFUNCTION()
		class UCharacterSubsystem* GetCharacterSubsystem();
	UFUNCTION()
		class UMissionSubsystem* GetMissionSubsystem();

	
};
