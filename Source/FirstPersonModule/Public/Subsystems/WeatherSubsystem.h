// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/WorldSubsystem.h"
#include "WeatherSubsystem.generated.h"

/**
 * 
 */
UCLASS()
class FIRSTPERSONMODULE_API UWeatherSubsystem : public UTickableWorldSubsystem
{
	GENERATED_BODY()
public:



private:	

	/*time*/
	UPROPERTY()
		float Time;

	/*temparature*/
	UPROPERTY()
		float Tempature; //in celsius

	/*rain*/
	UPROPERTY(meta = (MinValue = 0.0f, MaxValue = 1.0f))
		float RainIntensity;
	/*controls puddles and other things*/
	UPROPERTY(meta = (MinValue = 0.0f, MaxValue = 1.0f))
		float RainAccumlation;

	/*control the SOUND of lightning*/
	UPROPERTY(meta = (MinValue = 0.0f, MaxValue = 1.0f))
		float ThunderIntensity;
	/*controls the VISUAL intensity*/
	UPROPERTY(meta = (MinValue = 0.0f, MaxValue = 1.0f))
		float LightningIntensity;
	/*how often a lightning strike will happen*/
	UPROPERTY(meta = (MinValue = 0.0f, MaxValue = 1.0f))
		float LightningFrequency;
	/*how far away the lightning is?*/
	UPROPERTY(meta = (MinValue = 0.0f, MaxValue = 1.0f))
		float LightningDistance;


	/*snow*/
	UPROPERTY(meta = (MinValue = 0.0f, MaxValue = 1.0f))
		float FrostLevel;
	/*controls the level of snow on surfaces*/
	UPROPERTY(meta=(MinValue=0.0f, MaxValue=1.0f))
		float SnowAccumlation;
	/*controls the amount of snow falling from the sky*/
	UPROPERTY(meta = (MinValue = 0.0f, MaxValue = 1.0f))
		float SnowIntensity;

	/*for radiation storms*/
	UPROPERTY(meta = (MinValue = 0.0f, MaxValue = 1.0f))
		float RadiationLevel;
	UPROPERTY(meta = (MinValue = 0.0f, MaxValue = 1.0f))
		float RadiatedRainfall;

	/*acid rain!*/
	UPROPERTY(meta = (MinValue = 0.0f, MaxValue = 1.0f))
		float RainAcidity;



	//=================================================================================================================
	//====================================================FUNCTIONS====================================================
	//=================================================================================================================
	virtual TStatId GetStatId() const override { RETURN_QUICK_DECLARE_CYCLE_STAT(UWeatherSubsystem, STATGROUP_Tickables); }
	
	
	UFUNCTION(BlueprintPure, Category = "Tempature")
		float GetTempatureAsFahrenheit();
	UFUNCTION(BlueprintPure, Category = "Tempature")
		float GetTempatureAsCelsius();
	
};
