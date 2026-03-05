// Fill out your copyright notice in the Description page of Project Settings.


#include "Interactives/Actors/ElevatorCallButton.h"
#include "Components/PushButtonComponent.h"


AElevatorCallButton::AElevatorCallButton(const FObjectInitializer& ObjectInitializer)
	:Super(ObjectInitializer)
{
	if (InteractiveCollision)
	{
		InteractiveCollision->SetBoxExtent(FVector(1, 2, 2));
		InteractiveCollision->SetRelativeLocation(FVector(3, 0, 0));
	}
	PushButton = ObjectInitializer.CreateDefaultSubobject<UPushButtonComponent>(this, TEXT("PushButton"));
	if (PushButton)
	{
		PushButton->SetupAttachment(Mesh);
		PushButton->bHighlightOnHover = true;
	}
}

void AElevatorCallButton::BeginPlay()
{
	Super::BeginPlay();

	if (InteractiveCollision)
	{
		InteractiveCollision->OnInteractionStarted.AddDynamic(this, &AElevatorCallButton::OnButtonPressed);
		InteractiveCollision->OnInteractiveHovered.AddDynamic(this, &AElevatorCallButton::OnButtonHovered);
		InteractiveCollision->OnInteractiveUnhovered.AddDynamic(this, &AElevatorCallButton::OnButtonUnhovered);
	}
}


void AElevatorCallButton::OnButtonPressed(AFirstPersonCharacter* User, const UInteractiveAction* Action)
{

	if (OnElevatorCalled.IsBound())
		OnElevatorCalled.Broadcast(User, Action);

	/*notify blueprint*/
	BP_OnElevatorCalled(User, Action);
}

void AElevatorCallButton::OnButtonHovered(AFirstPersonCharacter* User)
{
	if (PushButton)
		PushButton->OnHovered(User);
}

void AElevatorCallButton::OnButtonUnhovered(AFirstPersonCharacter* User)
{
	if (PushButton)
		PushButton->OnUnhovered(User);
}


