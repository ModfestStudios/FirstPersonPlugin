// Fill out your copyright notice in the Description page of Project Settings.


#include "Components/ElevatorDestinationComponent.h"
#include "Interactives/Actors/Elevator.h"

UElevatorDestinationComponent::UElevatorDestinationComponent(const FObjectInitializer& ObjectInitializer)
	:Super(ObjectInitializer)
{
	bIsEditorOnly = false;
	bHiddenInGame = true;
	SetCollisionEnabled(ECollisionEnabled::NoCollision);
	SetCollisionResponseToAllChannels(ECR_Ignore);
}


#if WITH_EDITOR
void UElevatorDestinationComponent::PostEditChangeProperty(FPropertyChangedEvent& e)
{
	Super::PostEditChangeProperty(e);

}

void UElevatorDestinationComponent::OnComponentCreated()
{
	Super::OnComponentCreated();


	if (AElevator* Elevator = Cast<AElevator>(GetOwner()))
	{
		/*copy parent mesh*/
		if (Elevator->Mesh)
			SetStaticMesh(Elevator->Mesh->GetStaticMesh());
	}

	

}
#endif

