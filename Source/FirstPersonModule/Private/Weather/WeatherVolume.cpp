// Fill out your copyright notice in the Description page of Project Settings.


#include "Weather/WeatherVolume.h"

/*characters*/
#include "Characters/FirstPersonCharacter.h"

/*components*/
#include "Components/TemperatureComponent.h"
#include "Components/VitalsComponent.h"

/*framework*/
#include "GameFramework/Actor.h"

AWeatherVolume::AWeatherVolume(const FObjectInitializer& ObjectInitializer)
{	
	TemperatureComponent = ObjectInitializer.CreateDefaultSubobject<UTemperatureComponent>(this, TEXT("Temperature Component"));

	OnActorBeginOverlap.AddDynamic(this, &AWeatherVolume::OnBeginOverlap);
	OnActorEndOverlap.AddDynamic(this, &AWeatherVolume::OnEndOverlap);
}

void AWeatherVolume::OnBeginOverlap(AActor* OverlappedActor, AActor* OtherActor)
{
	if (AFirstPersonCharacter* Character = Cast<AFirstPersonCharacter>(OtherActor))
	{
		Character->GetVitalsComponent()->AddTemperatureInfluence(TemperatureComponent);
	}
}

void AWeatherVolume::OnEndOverlap(AActor* OverlappedActor, AActor* OtherActor)
{
	if (AFirstPersonCharacter* Character = Cast<AFirstPersonCharacter>(OtherActor))
	{
		Character->GetVitalsComponent()->RemoveTemperatureInfluence(TemperatureComponent);
	}
}
