// Fill out your copyright notice in the Description page of Project Settings.


#include "Subsystems/WeatherSubsystem.h"

float UWeatherSubsystem::GetTempatureAsFahrenheit()
{
    return (Tempature * 9.0f / 5.0f) + 32.0f;
}

float UWeatherSubsystem::GetTempatureAsCelsius()
{
    return Tempature;
}
