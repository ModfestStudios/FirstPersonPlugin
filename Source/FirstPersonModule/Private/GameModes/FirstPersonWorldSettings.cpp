// Fill out your copyright notice in the Description page of Project Settings.


#include "GameModes/FirstPersonWorldSettings.h"


#if WITH_EDITOR
void AFirstPersonWorldSettings::PostEditChangeProperty(FPropertyChangedEvent& ChangeEvent)
{
	FName PropertyName = (ChangeEvent.Property != NULL ? ChangeEvent.Property->GetFName() : NAME_None);

	if (PropertyName == GET_MEMBER_NAME_CHECKED(AFirstPersonWorldSettings, AmbientTemperature))
	{
		float Fahrenheit = AmbientTemperature * 1.8 + 32;
		Fahrenheit *= 100;
		Fahrenheit = FMath::TruncateToHalfIfClose(Fahrenheit);
		Fahrenheit = Fahrenheit / 100;
		
		AmbientTemperatureFahrenheit = Fahrenheit;
	}


	Super::PostEditChangeProperty(ChangeEvent);
}
#endif

float AFirstPersonWorldSettings::GetAmbientTemperature()
{
	return AmbientTemperature;	
}
