// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "PlayerHealthWidget.generated.h"

/**
 * 
 */
UCLASS(abstract)
class FIRSTPERSONMODULE_API UPlayerHealthWidget : public UUserWidget
{
	GENERATED_BODY()
public:


	UPROPERTY(EditAnywhere, Category = "Temperature")
		bool bDisplayFahrenheitTemperature;


	//=====================================================================================================================================================
	//======================================================================FUNCTIONS======================================================================
	//=====================================================================================================================================================

	UPROPERTY(EditAnywhere, Category = "Player Stats|Health", meta = (DisplayName = "Health Color - Healthy"))
		FLinearColor HealthColorHealthy = FLinearColor::FLinearColor(0.046776f, 0.578125f, 0.093811f, 1.000000f);
	UPROPERTY(EditAnywhere, Category = "Player Stats|Health", meta = (DisplayName = "Health Color - Scuffed"))
		FLinearColor HealthColorScuffed = FLinearColor::FLinearColor(0.499294f, 0.666667f, 0.158582f, 1.000000f);
	UPROPERTY(EditAnywhere, Category = "Player Stats|Health", meta = (DisplayName = "Health Color - Hurt"))
		FLinearColor HealthColorHurting = FLinearColor::FLinearColor(0.951584f, 0.989583f, 0.095146f, 1.000000f);
	UPROPERTY(EditAnywhere, Category = "Player Stats|Health", meta = (DisplayName = "Health Color - Injured"))
		FLinearColor HealthColorInjured = FLinearColor::FLinearColor(0.989583f, 0.535449f, 0.103501f, 1.000000f);
	UPROPERTY(EditAnywhere, Category = "Player Stats|Health", meta = (DisplayName = "Health Color - Severe"))
		FLinearColor HealthColorSevere = FLinearColor::FLinearColor(1.0f, 0.535449f, 0.0f, 1.000000f);
	UPROPERTY(EditAnywhere, Category = "Player Stats|Health", meta = (DisplayName = "Health Color - Critical"))
		FLinearColor HealthColorCritical = FLinearColor::FLinearColor(0.822917f, 0.0f, 0.007703f, 1.000000f);


	UPROPERTY(EditAnywhere, Category = "Player Stats|Temperature", meta = (DisplayName = "Temperature Color - Hyperthermic"))
		FLinearColor TemperatureColorHyperthermic = FLinearColor::FLinearColor(0.770833f, 0.012755f, 0.012044f, 1.000000f);
	UPROPERTY(EditAnywhere, Category = "Player Stats|Temperature", meta = (DisplayName = "Temperature Color - Hot"))
		FLinearColor TemperatureColorHot = FLinearColor::FLinearColor(0.770833f, 0.012755f, 0.012044f, 1.000000f);
	UPROPERTY(EditAnywhere, Category = "Player Stats|Temperature", meta = (DisplayName = "Temperature Color - Warm"))
		FLinearColor TemperatureColorWarm = FLinearColor::FLinearColor(0.932292f,0.413318f,0.082547f,1.000000f);
	UPROPERTY(EditAnywhere, Category = "Player Stats|Temperature", meta = (DisplayName = "Temperature Color - Normal"))
		FLinearColor TemperatureColorNeutral = FLinearColor::FLinearColor(0.796875f, 0.773010f,0.701416f, 1.000000f);
	UPROPERTY(EditAnywhere, Category = "Player Stats|Temperature", meta = (DisplayName = "Temperature Color - Cold"))
		FLinearColor TemperatureColorCold = FLinearColor::FLinearColor(0.162241f, 0.528483f, 0.973445f, 1.000000f);
	UPROPERTY(EditAnywhere, Category = "Player Stats|Temperature", meta = (DisplayName = "Temperature Color - Freezing"))
		FLinearColor TemperatureColorFreezing = FLinearColor::FLinearColor(0.034668f,0.037981f,0.739583f,1.000000f);
	UPROPERTY(EditAnywhere, Category = "Player Stats|Temperature", meta = (DisplayName = "Temperature Color - Hypothermic"))
		FLinearColor TemperatureColorHypothermic = FLinearColor::FLinearColor(0.034668f, 0.037981f, 0.739583f, 1.000000f);



	//===============================================================================================================================
	//===========================================================FUNCTIONS===========================================================
	//===============================================================================================================================

	//===================
	//=======STATS=======
	//===================

	UFUNCTION(BlueprintPure, Category = "Player Stats")
		float GetPlayerHealth();
	UFUNCTION(BlueprintPure, Category = "Player Stats")
		float GetPlayerStamina();
	UFUNCTION(BlueprintPure, Category = "Player Stats")
		float GetPlayerEncumberance();	
	UFUNCTION(BlueprintPure, Category = "Player Stats")
		float GetPlayerBloodLevel();
	UFUNCTION(BlueprintPure, Category = "Player Stats")
		float GetPlayerHydrationLevel();
	UFUNCTION(BlueprintPure, Category = "Player Stats")
		float GetPlayerEnergyLevel();

	UFUNCTION(BlueprintPure, Category = "Player Stats")
		float GetPlayerTemperature();
	UFUNCTION(BlueprintPure, Category = "Player Stats")
		float GetPlayerTemperatureFahrenheit();
	UFUNCTION(BlueprintPure, Category = "Player Stats")
		float GetPlayerTemperatureNormal();
	UFUNCTION(BlueprintPure, Category = "Player Stats")
		float GetAmbientTemperature();
	UFUNCTION(BlueprintPure, Category = "Player Stats")
		float GetAmbientTemperatureFahrenheit();
	UFUNCTION(BlueprintPure, Category = "Player Stats")
		float GetHeatChange();


	//======================
	//========COLORS========
	//======================
	UFUNCTION(BlueprintPure, Category = "Player Stats|Colors")
		FLinearColor GetHealthColor();
	UFUNCTION(BlueprintPure, Category = "Player Stats|Colors")
		FLinearColor GetTemperatureColor();



	UFUNCTION(BlueprintPure, Category = "Utilities")
		class AFirstPersonCharacter* GetOwningCharacter();
	UFUNCTION(BlueprintPure, Category = "Utilities")
		class UVitalsComponent* GetVitalsComponent();
	
};
