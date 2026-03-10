// Fill out your copyright notice in the Description page of Project Settings.


#include "Subsystems/WeatherSubsystem.h"

/*engine*/
#include "Engine/World.h"
#include "Engine/DeveloperSettings.h"
#include "Weather/WeatherSettings.h"
#include "GameFramework/WorldSettings.h"
#include "GameModes/FirstPersonWorldSettings.h"



void UWeatherSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
    Super::Initialize(Collection);

    /*initialize Weather Settings into WeatherSubsystem*/
    const UWeatherSettings* WeatherSettings = GetDefault<UWeatherSettings>();
    if (WeatherSettings && WeatherSettings->GetTemperatureZones().Num() > 0)
    {
        for (const FTemperatureZone& TempZone : WeatherSettings->GetTemperatureZones())
        {
            TemperatureZones.Add(TempZone);
        }
    }
}

void UWeatherSubsystem::OnWorldBeginPlay(UWorld& InWorld)
{
    Super::OnWorldBeginPlay(InWorld);

    if (AFirstPersonWorldSettings* WorldSettings = Cast<AFirstPersonWorldSettings>(GetWorld()->GetWorldSettings()))
    {
        AmbientTemperature = WorldSettings->GetAmbientTemperature();
    }
}

float UWeatherSubsystem::GetTempatureAsFahrenheit()
{
    return CeliusToFahrenheit(AmbientTemperature);
}

float UWeatherSubsystem::GetTempatureAsCelsius()
{
    return AmbientTemperature;
}

float UWeatherSubsystem::CeliusToFahrenheit(float Celsius)
{
    return (Celsius * 9.0f / 5.0f) + 32.0f;
}


float UWeatherSubsystem::GetAmbientTemperatureEffectRate()
{
    return GetTemperatureEffectRate(AmbientTemperature);
}

float UWeatherSubsystem::GetTemperatureEffectRate(int32 Temperature)
{
    for (uint8 i = 0; i < TemperatureZones.Num(); i++)
    {
        int32 min = TemperatureZones[i].Min;
        int32 max = TemperatureZones[i].Max;

        if (FMath::IsWithinInclusive(AmbientTemperature, min, max))
            return TemperatureZones[i].ChangeRate;
    }

    return 0.0f;
}
