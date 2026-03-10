// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DeveloperSettings.h"
#include "Subsystems/WeatherSubsystem.h"
#include "WeatherSettings.generated.h"

/**
 * 
 */
UCLASS(config=Game, DefaultConfig)
class FIRSTPERSONMODULE_API UWeatherSettings : public UDeveloperSettings
{
	GENERATED_BODY()
public:

protected:
	UPROPERTY(Config, EditAnywhere, Category = "Temperature")
		TArray<FTemperatureZone> TemperatureZones;
	

public:
	UWeatherSettings(const FObjectInitializer& ObjectInitializer);

#if WITH_EDITOR
	virtual void PostEditChangeChainProperty(FPropertyChangedChainEvent& Event) override;
#endif

	virtual FName GetCategoryName() const override;
#if WITH_EDITOR
	virtual FText GetSectionText() const override;
	virtual FText GetSectionDescription() const override;	
#endif


public:
	UFUNCTION()
		const TArray<FTemperatureZone>& GetTemperatureZones() const;



};
