// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Engine/GameEngine.h"
#include "UserSettingsWidget.generated.h"



/**
 * 
 */
UCLASS()
class FIRSTPERSONMODULE_API UUserSettingsWidget : public UUserWidget
{
	GENERATED_BODY()
public:




	//=======================================================================================================================
	//=======================================================FUNCTIONS=======================================================
	//=======================================================================================================================

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "User Settings|Graphics")
		FIntPoint GetScreenResolution();

	/*returns the current monitor's resolution*/
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "User Settings|Graphicc")
		FIntPoint GetDesktopResolution();

	/*gets a list of this monitor's supported screen sizes*/
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "User Settings|Graphics")
		TArray<FIntPoint> GetSupportedScreenResolutions();

	UFUNCTION(BlueprintCallable, Category = "User Settings|Graphics")
		virtual void SetScreenResolution(FIntPoint NewResolution);
	

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "User Settings|Graphics")
		EWindowMode::Type GetWindowMode();
	UFUNCTION(BlueprintCallable, Category = "User Settings|Graphics")
		virtual void SetWindowMode(EWindowMode::Type Mode);





	//=========================
	//========UTILITIES========
	//=========================
public:
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "User Settings")
		class UGameUserSettings* GetUserSettings() { return GEngine->GameUserSettings; }


	
};
