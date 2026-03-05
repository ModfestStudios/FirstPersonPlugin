// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "Voice.h"
#include "FirstPersonPlayerController.generated.h"


/**
 * 
 */
UCLASS()
class FIRSTPERSONMODULE_API AFirstPersonPlayerController : public APlayerController
{
	GENERATED_BODY()
public:


	/*voicechat*/
	//IVoice VoiceInterface;
	UPROPERTY(BlueprintReadOnly, Category = "Online|Voice Chat")
		bool bVoiceChatEnabled = false;

	/*input*/
	UPROPERTY(EditDefaultsOnly, Category = "Input")
		TObjectPtr<class UInputMappingContext> ControllerMappingContext;

	UPROPERTY(EditDefaultsOnly, Category = "Input")
		TObjectPtr<class UInputAction> ToggleServerInfoAction;
	UPROPERTY(EditDefaultsOnly, Category = "Input")
		TObjectPtr<class UInputAction> ToggleEscapeMenuAction;	

	/*ui*/
public:
	UPROPERTY(BlueprintReadOnly, Category = "UI")
		class UUserWidget* EscapeMenuWidget;
	/*server info screen*/
	UPROPERTY(BlueprintReadOnly, Category = "UI")
		class UUserWidget* ServerInfoWidget;
	UPROPERTY(BlueprintReadOnly, Category = "UI")
		class UUserWidget* LoadoutWidget;


	//===============================================================================================================
	//===================================================FUNCTIONS===================================================
	//===============================================================================================================
	AFirstPersonPlayerController();

	virtual void BeginPlay() override;
	virtual void SetupInputComponent() override;

	/*event called when a client gets a new pawn*/
	virtual void AcknowledgePossession(class APawn* P) override;


	//====================================
	//=============VOICE CHAT=============
	//====================================
//protected:
//	UFUNCTION(Client, Reliable)
//		virtual void Client_InitVoiceInterface();
//	UFUNCTION(BlueprintCallable, Category = "Voice Chat")
//		virtual void ToggleVoiceChat();
//	UFUNCTION(BlueprintCallable, Category = "Voice Chat")
//		virtual void EnableVoiceChat();
//	UFUNCTION(BlueprintCallable, Category = "Voice Chat")
//		virtual void DisableVoiceChat();

	//=================================
	//============SPECTATOR============
	//=================================

public:
	UFUNCTION(BlueprintCallable, Category = "Spectator")
	virtual void EnterSpectatorMode();

	//=========================================
	//===============ESCAPE MENU===============
	//=========================================
public:
	UFUNCTION(BlueprintCallable, Category = "UI")
		virtual void InitializeEscapeMenuWidget(TSubclassOf<class UUserWidget> WidgetClass);
	UFUNCTION()
		virtual void ToggleEscapeMenu();
	UFUNCTION(BlueprintCallable, Category = "UI")
		virtual void OpenEscapeMenuWidget();
	UFUNCTION(BlueprintCallable, Category = "UI")
		virtual void CloseEscapeMenuWidget();

	//========================================
	//================LOADOUTS================
	//========================================
public:
	UFUNCTION(BlueprintCallable, Category = "UI")
		virtual void InitializeLoadoutWidget(TSubclassOf<class ULoadoutWidget> WidgetClass);
	UFUNCTION(BlueprintCallable, Category = "UI")
		virtual void OpenLoadoutWidget();
	UFUNCTION(BlueprintCallable, Category = "UI")
		virtual void CloseLoadoutWidget();


	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Loadout")
		virtual class AInventoryLoadout* GetLoadoutInfo();
	UFUNCTION(BlueprintCallable, Category = "Loadout")
		virtual void SetLoadoutItem_PrimaryWeapon(TSubclassOf<AActor> Weapon);


	//=======================================
	//==============SERVER INFO==============
	//=======================================
public:
	UFUNCTION(BlueprintCallable, Category = "UI")
		virtual void InitializeServerInfoWidget(TSubclassOf<UUserWidget> WidgetClass);
	UFUNCTION()
		void ToggleServerInfo();
	UFUNCTION()
		void OpenServerInfoWidget();
	UFUNCTION()
		void CloseServerInfoWidget();


	//===================================
	//===========MAP SELECTION===========
	//===================================
	UFUNCTION(Server, Reliable, WithValidation, BlueprintCallable, Category = "Map Selecting")
		void ServerSelectNextMap(class UMapAsset* RequestedMap);



	//=================================
	//=========LEVEL STREAMING=========
	//=================================
	/*notify the server a client has finished loading*/
	UFUNCTION(Server, Reliable)
		void ServerNotify_LevelInstanceLoadFinished();

	UFUNCTION(Server, Unreliable)
		void Server_UpdateLevelLoadPercent(float NewPercent);
	
};
