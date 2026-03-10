// Fill out your copyright notice in the Description page of Project Settings.


#include "Weather/WeatherSettings.h"
#include "Subsystems/WeatherSubsystem.h"

UWeatherSettings::UWeatherSettings(const FObjectInitializer& ObjectInitializer)
	:Super(ObjectInitializer)
{

	/*default initialization if does not exist in config file*/
	if (TemperatureZones.Num() == 0)
	{

		TemperatureZones.Add(FTemperatureZone(ETemperatureZone::ColdZone3, FName("Hyperthermia"), -50, -1, -0.01f)); //cold zone 3 (-58 to 30.2F)
		TemperatureZones.Add(FTemperatureZone(ETemperatureZone::ColdZone2, FName("Freezing"), 0, 9, -0.005f)); //cold zone 2 (32 to 48.2F)
		TemperatureZones.Add(FTemperatureZone(ETemperatureZone::ColdZone1,FName("Cold"), 10, 15, -0.0025f)); //cold zone 1 (50 to 59F)

		TemperatureZones.Add(FTemperatureZone(ETemperatureZone::Neutral,"Neutral Zone", 16, 32, 0.0f)); //totally fine range, no change (60 to 90F)

		TemperatureZones.Add(FTemperatureZone(ETemperatureZone::HeatZone1,FName("Warm"), 33, 37, 0.0025f)); //heat zone 1 (91.4 to 98.6F)
		TemperatureZones.Add(FTemperatureZone(ETemperatureZone::HeatZone2,FName("Hot"), 38, 43, 0.005f)); //heat zone 2 (100.4 to 109.4F)
		TemperatureZones.Add(FTemperatureZone(ETemperatureZone::HeatZone3,FName("Hyperthermia"), 44, 50, 0.01f)); //heat zone 3 (111.2 to 122F)

		SaveConfig();
	}
}


#if WITH_EDITOR
void UWeatherSettings::PostEditChangeChainProperty(FPropertyChangedChainEvent& Event)
{
	Super::PostEditChangeChainProperty(Event);

	static const FName MinName = GET_MEMBER_NAME_CHECKED(FTemperatureZone, Min);
	static const FName MaxName = GET_MEMBER_NAME_CHECKED(FTemperatureZone, Max);

	if (Event.Property && Event.Property->GetFName() == MinName)
	{
		for (FTemperatureZone& Zone : TemperatureZones)
		{
			Zone.MinFahrenheit = Zone.CelsiusToFahrenheit(Zone.Min);
		}
	}
	if (Event.Property && Event.Property->GetFName() == MaxName)
	{
		for (FTemperatureZone& Zone : TemperatureZones)
		{
			Zone.MaxFahrenheit = Zone.CelsiusToFahrenheit(Zone.Max);
		}
	}
}
#endif



FName UWeatherSettings::GetCategoryName() const
{
	return TEXT("Project");
}

#if WITH_EDITOR
FText UWeatherSettings::GetSectionText() const
{
	return NSLOCTEXT("WeatherSettings", "WeatherCategory", "Weather");
}

FText UWeatherSettings::GetSectionDescription() const
{
	return NSLOCTEXT("Weathersettings", "WeatherCategoryDescription", "Weather Controls");
}

#endif

const TArray<FTemperatureZone>& UWeatherSettings::GetTemperatureZones() const
{
	return TemperatureZones;
}

