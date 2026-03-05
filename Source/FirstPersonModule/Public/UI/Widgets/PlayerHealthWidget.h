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

	UPROPERTY(EditAnywhere, Category = "Player Stats|Health", meta = (DisplayName = "Health Color - Healthy"))
		FLinearColor HealthColorHealthy = FLinearColor::FLinearColor(0.046776, 0.578125, 0.093811, 1.000000);
	UPROPERTY(EditAnywhere, Category = "Player Stats|Health", meta = (DisplayName = "Health Color - Scuffed"))
		FLinearColor HealthColorScuffed = FLinearColor::FLinearColor(0.499294, 0.666667, 0.158582, 1.000000);
	UPROPERTY(EditAnywhere, Category = "Player Stats|Health", meta = (DisplayName = "Health Color - Hurt"))
		FLinearColor HealthColorHurting = FLinearColor::FLinearColor(0.951584, 0.989583, 0.095146, 1.000000);
	UPROPERTY(EditAnywhere, Category = "Player Stats|Health", meta = (DisplayName = "Health Color - Injured"))
		FLinearColor HealthColorInjured = FLinearColor::FLinearColor(0.989583, 0.535449, 0.103501, 1.000000);
	UPROPERTY(EditAnywhere, Category = "Player Stats|Health", meta = (DisplayName = "Health Color - Severe"))
		FLinearColor HealthColorSevere = FLinearColor::FLinearColor(1.0, 0.535449, 0.0, 1.000000);
	UPROPERTY(EditAnywhere, Category = "Player Stats|Health", meta = (DisplayName = "Health Color - Critical"))
		FLinearColor HealthColorCritical = FLinearColor::FLinearColor(0.822917, 0.0, 0.007703, 1.000000);



	UPROPERTY(EditAnywhere, Category = "Player Stats|Temperature")
		FLinearColor TemperatureColorHot = FLinearColor::FLinearColor(0.770833, 0.012755, 0.012044, 1.000000);
	UPROPERTY(EditAnywhere, Category = "Player Stats|Temperature")
		FLinearColor TemperatureColorWarm = FLinearColor::FLinearColor(0.932292,0.413318,0.082547,1.000000);
	UPROPERTY(EditAnywhere, Category = "Player Stats|Temperature")
		FLinearColor TemperatureColorNeutral = FLinearColor::FLinearColor(0.796875, 0.773010,0.701416, 1.000000);
	UPROPERTY(EditAnywhere, Category = "Player Stats|Temperature")
		FLinearColor TemperatureColorCold = FLinearColor::FLinearColor(0.162241, 0.528483, 0.973445, 1.000000);
	UPROPERTY(EditAnywhere, Category = "Player Stats|Temperature")
		FLinearColor TemperatureColorFreezing = FLinearColor::FLinearColor(0.034668,0.037981,0.739583,1.000000);




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
		float GetPlayerTemperatureNormal();

	//======================
	//========COLORS========
	//======================
	UFUNCTION(BlueprintPure, Category = "Player Stats|Colors")
		FLinearColor GetHealthColor();
	UFUNCTION(BlueprintPure, Category = "Player Stats|Colors")
		FLinearColor GetTemperatureColor();



	UFUNCTION(BlueprintPure, Category = "Utilities")
		class AFirstPersonCharacter* GetOwningCharacter();
	
};
