// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/Widgets/PlayerHealthWidget.h"
#include "Characters/FirstPersonCharacter.h"



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



float UPlayerHealthWidget::GetPlayerTemperatureNormal()
{
	FVector2D inputRange(40.0f, 90.0f);
	FVector2D outputRange(0.0f, 1.0f);

	return FMath::GetMappedRangeValueClamped(inputRange, outputRange, GetPlayerTemperature());
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
	float Temp = GetPlayerTemperature();

	if (Temp <= 40.0f)
		return TemperatureColorFreezing;
	if (Temp <= 55.0f)
		return TemperatureColorCold;
	if (Temp <= 75.0f)
		return TemperatureColorNeutral;
	if (Temp <= 90.0f)
		return TemperatureColorWarm;
	if (Temp > 90.0f)
		return TemperatureColorHot;

	/*default*/
	return TemperatureColorNeutral;
}

AFirstPersonCharacter* UPlayerHealthWidget::GetOwningCharacter()
{
	return GetOwningPlayerPawn<AFirstPersonCharacter>();
}
