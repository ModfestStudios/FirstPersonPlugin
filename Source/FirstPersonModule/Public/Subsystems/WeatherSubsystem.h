// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/WorldSubsystem.h"
#include "WeatherSubsystem.generated.h"

UENUM(BlueprintType)
enum class ETemperatureZone : uint8
{
	Neutral,
	ColdZone1,
	ColdZone2,
	ColdZone3,
	ColdZone4,
	ColdZone5,
	ColdZone6,
	HeatZone1,
	HeatZone2,
	HeatZone3,
	HeatZone4,
	HeatZone5,
	HeatZone6
};

USTRUCT(BlueprintType)
struct FTemperatureZone
{
	GENERATED_BODY()
public:

	UPROPERTY(Config, EditAnywhere, Category = "Temperature")
		ETemperatureZone Zone;
	UPROPERTY(Config, EditAnywhere, Category = "Temperature")
		FName Name;
	UPROPERTY(Config, EditAnywhere, Category = "Temperature", meta = (Units = "Celsius"))
		int32 Min;
	UPROPERTY(VisibleAnywhere, Category = "Temperature", meta = (DisplayName = "Min (Fahrenheit)", ForceUnits="Farenheit"))
		int32 MinFahrenheit;
	UPROPERTY(Config,EditAnywhere, Category = "Temperature", meta = (Units = "Celsius"))
		int32 Max;
	UPROPERTY(VisibleAnywhere, Category = "Temperature", meta = (DisplayName = "Max (Fahrenheit)", ForceUnits="Farenheit"))
		int32 MaxFahrenheit;
	UPROPERTY(Config,EditAnywhere, Category = "Temperature")
		float ChangeRate;


	FTemperatureZone() {
		Zone = ETemperatureZone::Neutral;
		Name = "Neutral Zone";
		Min = 16;
		MinFahrenheit = CelsiusToFahrenheit(16);
		Max = 32;
		MaxFahrenheit = CelsiusToFahrenheit(32);
		ChangeRate = 0.0f;
	}
	FTemperatureZone(ETemperatureZone Zone,FName Name, int32 TemperatureMin, int32 TemperatureMax, float RateOfChange)
	{
		this->Zone = Zone;
		this->Name = Name;
		Min = TemperatureMin;
		MinFahrenheit = CelsiusToFahrenheit(TemperatureMin);
		Max = TemperatureMax;
		MaxFahrenheit = CelsiusToFahrenheit(TemperatureMax);
		ChangeRate = RateOfChange;
	}

	
	float CelsiusToFahrenheit(float Celsius) { return (Celsius * 9.0f / 5.0f) + 32.0f; }
};

USTRUCT(BlueprintType)
struct FTemperatureZoneEffect
{
	GENERATED_BODY()
public:

	UPROPERTY(EditAnywhere, Category = "Temperature")
		FName TemperatureZone;
	/*prevents the body temperature from going beyond this by ambients alone*/
	UPROPERTY(EditAnywhere, Category = "Temperature")
		float BodyTemperatureCap;
};

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

	/**************/
	/*temparature*/
	/**************/
	UPROPERTY()
		float AmbientTemperature; //in celsius

	
	UPROPERTY()
		TArray<FTemperatureZone> TemperatureZones;


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
	

	virtual void Initialize(FSubsystemCollectionBase& Collection) override;
		
	virtual void OnWorldBeginPlay(UWorld& InWorld) override;
	

public:
	UFUNCTION(BlueprintPure, Category = "Tempature")
		float GetTempatureAsFahrenheit();
	UFUNCTION(BlueprintPure, Category = "Tempature")
		float GetTempatureAsCelsius();
	UFUNCTION(BlueprintPure, Category = "Temperature")
		float CeliusToFahrenheit(float Celsius);

public:

	/*the ambient temperature effect */
	UFUNCTION(BlueprintPure, BlueprintCallable, Category = "Temperature")
		float GetAmbientTemperatureEffectRate();
	UFUNCTION(BlueprintPure, BlueprintCallable, Category = "Temperature")
		float GetTemperatureEffectRate(int32 Temperature);
	
};
