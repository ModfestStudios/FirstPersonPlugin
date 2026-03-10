// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/Widgets/PlayerHealthWidget.h"
#include "Characters/FirstPersonCharacter.h"
#include "Components/VitalsComponent.h"

/*engine*/
#include "Engine/World.h"

/*subsystems*/
#include "Subsystems/WeatherSubsystem.h"


//===================
//=======STATS=======
//===================

float UPlayerHealthWidget::GetPlayerHealth()
{
	return GetOwningCharacter() ? GetOwningCharacter()->GetHealth() : 0.0f;
}

float UPlayerHealthWidget::GetPlayerStamina()
{
	return GetOwningCharacter() ? GetOwningCharacter()->GetStamina() : 0.0f;		
}

float UPlayerHealthWidget::GetPlayerEncumberance()
{
	return GetOwningCharacter() ? GetOwningCharacter()->GetEncumbrance() : 0.0f;
}

float UPlayerHealthWidget::GetPlayerBloodLevel()
{
	return GetOwningCharacter() ? GetOwningCharacter()->GetBloodLevel() : 0.0f;
}

float UPlayerHealthWidget::GetPlayerHydrationLevel()
{
	return GetOwningCharacter() ? GetOwningCharacter()->GetHydrationLevel() : 0.0f;
}

float UPlayerHealthWidget::GetPlayerEnergyLevel()
{
	return GetOwningCharacter() ? GetOwningCharacter()->GetEnergyLevel() : 0.0f;
}

float UPlayerHealthWidget::GetPlayerTemperature()
{
	return GetOwningCharacter() ? GetOwningCharacter()->GetTemperature() : 0.0f;
	
}

float UPlayerHealthWidget::GetPlayerTemperatureFahrenheit()
{
	float Celsius = GetPlayerTemperature();
	
	if (UWeatherSubsystem* WSS = GetWorld()->GetSubsystem<UWeatherSubsystem>())
		return WSS->CeliusToFahrenheit(Celsius);
	else
		return Celsius;
}



float UPlayerHealthWidget::GetPlayerTemperatureNormal()
{
	FVector2D inputRange(40.0f, 105.0f);
	FVector2D outputRange(0.0f, 1.0f);

	return FMath::GetMappedRangeValueClamped(inputRange, outputRange, GetPlayerTemperature());
}

float UPlayerHealthWidget::GetAmbientTemperature()
{
	if (UVitalsComponent* VitalsComp = GetVitalsComponent())
	{
		return VitalsComp->GetAmbientTemperature();
	}

	return 0.0f;
}

float UPlayerHealthWidget::GetAmbientTemperatureFahrenheit()
{
	float Celsius = GetAmbientTemperature();

	if (UWeatherSubsystem* WSS = GetWorld()->GetSubsystem<UWeatherSubsystem>())
		return WSS->CeliusToFahrenheit(Celsius);
	else
		return Celsius;
}

float UPlayerHealthWidget::GetHeatChange()
{
	if (UVitalsComponent* VitalsComp = GetVitalsComponent())
	{
		return VitalsComp->GetHeatChange();
	}
	return 0.0f;
}

//======================
//========COLORS========
//======================


FLinearColor UPlayerHealthWidget::GetHealthColor()
{
	float Health = GetPlayerHealth();

	if (Health >= 100)
		return HealthColorHealthy;
	if (Health >= 85)
		return HealthColorScuffed;
	if (Health >= 65)
		return HealthColorHurting;
	if (Health >= 42)
		return HealthColorInjured;
	if (Health >= 25)
		return HealthColorSevere;
	if (Health >= 1)
		return HealthColorCritical;
	
	return FLinearColor();
}

FLinearColor UPlayerHealthWidget::GetTemperatureColor()
{
	UVitalsComponent* VitalsComp = GetVitalsComponent();
	if (!VitalsComp)
		return TemperatureColorNeutral;

	float Temp = GetPlayerTemperature();	

	if (Temp <= VitalsComp->HypothermiaThreshold)
		return TemperatureColorHypothermic;
	if (Temp <= VitalsComp->FreezingThreshold)
		return TemperatureColorFreezing;
	if (Temp <= VitalsComp->ColdThreshold)
		return TemperatureColorCold;
	if (Temp < VitalsComp->WarmThreshold && Temp > VitalsComp->ColdThreshold)
		return TemperatureColorNeutral;
	if (Temp >= VitalsComp->WarmThreshold)
		return TemperatureColorWarm;
	if (Temp >= VitalsComp->HotThreshold)
		return TemperatureColorHot;
	if (Temp >= VitalsComp->HyperthermiaThreshold)
		return TemperatureColorHyperthermic;

	/*default*/
	return TemperatureColorNeutral;
}

AFirstPersonCharacter* UPlayerHealthWidget::GetOwningCharacter()
{
	return GetOwningPlayerPawn<AFirstPersonCharacter>();
}

UVitalsComponent* UPlayerHealthWidget::GetVitalsComponent()
{
	if (AFirstPersonCharacter* Char = GetOwningCharacter())
		return Char->GetVitalsComponent();
	else
		return nullptr;
}
