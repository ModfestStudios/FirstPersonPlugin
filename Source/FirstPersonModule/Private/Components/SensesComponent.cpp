// Fill out your copyright notice in the Description page of Project Settings.


#include "Components/SensesComponent.h"

/*engine*/
#include "Engine/World.h"

/*framework*/
#include "GameFramework/Actor.h"

/*subsystems*/
#include "Subsystems/AIBehaviorSubsystem.h"

// Sets default values for this component's properties
USensesComponent::USensesComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}


// Called when the game starts
void USensesComponent::BeginPlay()
{
	Super::BeginPlay();


	if (GetWorld())
		GetWorld()->GetSubsystem<UAIBehaviorSubsystem>()->RegisterSensesComponent(this);
}


// Called every frame
void USensesComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}

TArray<FActorEvaluation> USensesComponent::GetAllKnownActors()
{
	return SensedActors;
}

float USensesComponent::GetDistanceToActor(AActor* Actor)
{
	if (!Actor || !GetOwner())
		return 0.0f;

	return GetOwner()->GetDistanceTo(Actor);
}

bool USensesComponent::IsWithinVisualRange(AActor* Actor)
{
	if (!Actor)
		return false;

	return GetDistanceToActor(Actor) <= MaxVisionDistance;
}

