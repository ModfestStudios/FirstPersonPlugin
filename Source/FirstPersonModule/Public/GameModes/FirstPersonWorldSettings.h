// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/WorldSettings.h"
#include "FirstPersonWorldSettings.generated.h"

/**
 * 
 */
UCLASS()
class FIRSTPERSONMODULE_API AFirstPersonWorldSettings : public AWorldSettings
{
	GENERATED_BODY()
public:

protected:
	UPROPERTY(EditAnywhere, Category = "Weather", meta = (Units="Celsius"))
		float AmbientTemperature = 29.5;
	UPROPERTY(VisibleAnywhere, Category = "Weather", meta = (DisplayName = "Ambient Temperature (Fahrenheit)"))
		float AmbientTemperatureFahrenheit;



	//===============================================================================================================================================================
	//===========================================================================FUNCTIONS===========================================================================
	//===============================================================================================================================================================


public:
	virtual void PostEditChangeProperty(FPropertyChangedEvent& ChangeEvent) override;



	UFUNCTION(BlueprintCallable, Category = "Weather|Temperature")
		float GetAmbientTemperature();







	
};
