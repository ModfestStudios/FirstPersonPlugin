// Fill out your copyright notice in the Description page of Project Settings.


#include "Interactives/Actors/IndicatorLight.h"

/*components*/
#include "Components/StaticMeshComponent.h"

/*materials*/
#include "Materials/MaterialInstanceDynamic.h"

/*network*/
#include "Net/UnrealNetwork.h"

/*utilities*/
#include "Kismet/GameplayStatics.h"


void AIndicatorLight::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AIndicatorLight, bIsTurnedOn);
	DOREPLIFETIME(AIndicatorLight, Intensity);
}

#if WITH_EDITOR
void AIndicatorLight::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent)
{	
	Super::PostEditChangeProperty(PropertyChangedEvent);

	if (MaterialInstance == nullptr)
		InitMaterialInstance();

	FName PropertyName = (PropertyChangedEvent.Property != nullptr) ? PropertyChangedEvent.Property->GetFName() : NAME_None;

	if (PropertyName == GET_MEMBER_NAME_CHECKED(AIndicatorLight, bIsTurnedOn))
	{
		if (bIsTurnedOn)
			OnTurnedOn();
		else
			OnTurnedOff();
	}
	if (PropertyName == GET_MEMBER_NAME_CHECKED(AIndicatorLight, LightColor))
	{
		SetLightColor(LightColor);
	}
	if (PropertyName == GET_MEMBER_NAME_CHECKED(AIndicatorLight, Intensity))
	{
		SetIntensity(Intensity);
	}

}
#endif

// Sets default values
AIndicatorLight::AIndicatorLight(const FObjectInitializer& ObjectInitializer)
{

	/*generate the mesh component for us to use*/
	Mesh = ObjectInitializer.CreateDefaultSubobject<UStaticMeshComponent>(this, TEXT("MeshComponent"));

	if (Mesh)
	{
		SetRootComponent(Mesh);
	}


	bReplicates = true;
	PrimaryActorTick.bCanEverTick = true;
}


// Called when the game starts or when spawned
void AIndicatorLight::BeginPlay()
{
	Super::BeginPlay();

	InitMaterialInstance();	
	
	/*initial state*/
	if (IsTurnedOn())
		OnTurnedOn();
	else
		OnTurnedOff();
}

// Called every frame
void AIndicatorLight::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AIndicatorLight::TurnOn()
{	
	bIsTurnedOn = true;
	OnTurnedOn();
	BP_OnTurnedOn();
}

void AIndicatorLight::TurnOff()
{
	bIsTurnedOn = false;
	OnTurnedOff();
	BP_OnTurnedOff();
}

void AIndicatorLight::Toggle()
{
	if (IsTurnedOn())
		TurnOff();
	else
		TurnOn();
}

bool AIndicatorLight::IsTurnedOn()
{
	return bIsTurnedOn;
}

void AIndicatorLight::OnRep_IndicatorToggled()
{
	if (IsTurnedOn())
		TurnOn();
	else
		TurnOff();
}

void AIndicatorLight::SetLightColor(FLinearColor NewColor)
{
	LightColor = NewColor;

	if(MaterialInstance)
	{
		MaterialInstance->SetVectorParameterValue(ColorParameterName, NewColor);
	}
}

void AIndicatorLight::SetIntensity(float NewIntensity)
{
	Intensity = NewIntensity;

	if (MaterialInstance)
	{
		MaterialInstance->SetScalarParameterValue(IntensityParameterName, NewIntensity);
	}
}

//====================
//=======EVENTS=======
//====================

void AIndicatorLight::OnTurnedOn()
{
	if (MaterialInstance)
	{
		MaterialInstance->SetScalarParameterValue(IntensityParameterName, Intensity);
	}
}

void AIndicatorLight::OnTurnedOff()
{
	if (MaterialInstance)
	{
		MaterialInstance->SetScalarParameterValue(IntensityParameterName, 0.0f);
	}
}

void AIndicatorLight::InitMaterialInstance()
{
	/*safety check*/
	if (LightMaterialName.IsNone() || MaterialInstance != nullptr || Mesh == nullptr)
		return;

	UMaterialInterface* MaterialBase = Mesh->GetMaterialByName(LightMaterialName);
	MaterialInstance = UMaterialInstanceDynamic::Create(MaterialBase, this);
	
	if (MaterialInstance)
	{
		Mesh->SetMaterialByName(LightMaterialName, MaterialInstance);
		MaterialInstance->SetVectorParameterValue(ColorParameterName, LightColor);
		MaterialInstance->SetScalarParameterValue(IntensityParameterName, Intensity);
	}
	
}

