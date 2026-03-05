// Fill out your copyright notice in the Description page of Project Settings.


#include "Components/BehaviorComponent.h"

#include "Characters/FirstPersonCharacter.h"
#include "AI/FirstPersonAIController.h"

// Sets default values for this component's properties
UBehaviorComponent::UBehaviorComponent()
{	
	PrimaryComponentTick.bCanEverTick = true;	
}


// Called when the game starts
void UBehaviorComponent::BeginPlay()
{
	Super::BeginPlay();

	InitBehaviorTree();	
}

void UBehaviorComponent::InitBehaviorTree()
{
	if (!BehaviorTree)
		return;

	if (APawn* P = Cast<APawn>(GetOwner()))
	{
		if (AFirstPersonAIController* AIC = Cast<AFirstPersonAIController>(P->GetController()))
			AIC->RunBehaviorTree(BehaviorTree);
	}
}


// Called every frame
void UBehaviorComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}

