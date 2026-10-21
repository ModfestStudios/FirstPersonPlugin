// Fill out your copyright notice in the Description page of Project Settings.


#include "Interactives/Actors/Door.h"
#include "Components/InteractiveCollisionComponent.h"
#include "Components/DoorComponent.h"
#include "Interactives/InteractiveAction.h"

#include "Interactives/Actions/OpenAction.h"
#include "Interactives/Actions/CloseAction.h"
#include "Interactives/Actions/LockAction.h"
#include "Interactives/Actions/UnlockAction.h"

#include "Net/UnrealNetwork.h"

// Sets default values

ADoor::ADoor(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer.SetDefaultSubobjectClass<UDoorComponent>(TEXT("Interactive Component")))
{	
	if (InteractiveComponent)
	{
		//SetRootComponent(DoorComponent);
		//InteractiveComponent->Rename(TEXT("Door Component"));
		InteractiveComponent->SetRelativeLocation(FVector(0, -60, 120));
		InteractiveComponent->InteractiveCollision->InitBoxExtent(FVector(10,60,120));
	}

	//Mesh = ObjectInitializer.CreateDefaultSubobject<UStaticMeshComponent>(this, TEXT("Mesh"));
	//if (Mesh)
	//{		
	//	//Mesh->SetRelativeLocation(FVector(0,60,-120));
	//}

	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	bReplicates = true;	
	NetPriority = 2.0f;
	SetNetUpdateFrequency(10.0f);
	SetMinNetUpdateFrequency(2.0f);
	PrimaryActorTick.bCanEverTick = true;
}

// Called when the game starts or when spawned
void ADoor::BeginPlay()
{
	Super::BeginPlay();
	
	DoorComponents.Empty();
	GetComponents<UDoorComponent>(DoorComponents);
}

// Called every frame
void ADoor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void ADoor::Open()
{
	for (UDoorComponent* Door : DoorComponents)
	{
		if (Door) //component already checks to make sure it's valid - we just want to tell it someone tried
			Door->OpenDoor(); 
	}
}

void ADoor::Close()
{
	for (UDoorComponent* Door : DoorComponents)
	{
		if (Door) //component already checks to make sure it's valid - we just want to tell someone tried
			Door->CloseDoor();
	}
}

//================================================
//====================ACTIONS=====================
//================================================

void ADoor::NativeOnInteractionStarted(AFirstPersonCharacter* User, UInteractiveComponent* Interactive, const UInteractiveAction* Action, const FInteractionContext& Context)
{
	Super::NativeOnInteractionStarted(User,Interactive,Action,Context);
}

void ADoor::NativeOnInteractionCompleted(AFirstPersonCharacter* User, UInteractiveComponent* Interactive, const UInteractiveAction* Action, const FInteractionContext& Context)
{
	if(Action->IsA<UOpenAction>())
		Open();
	if(Action->IsA<UCloseAction>())
		Close();
	
	Super::NativeOnInteractionCompleted(User, Interactive, Action, Context);
}

void ADoor::NativeOnInteractionCanceled(AFirstPersonCharacter* User, UInteractiveComponent* Interactive, const UInteractiveAction* Action, const FInteractionContext& Context)
{
	Super::NativeOnInteractionCanceled(User, Interactive, Action, Context);
}

bool ADoor::IsActionAvailable_Implementation(const AFirstPersonCharacter* User,const UInteractiveComponent* Interactive, const UInteractiveAction* Action) const
{
	if (!Interactive || !Action)
		return false;

	const UDoorComponent* DoorComponent = Cast<UDoorComponent>(Interactive);

	//if (Interactive != InteractiveComponent)
	//	return true;

	if (Action->IsA<UOpenAction>())
		return !DoorComponent->IsDoorOpen();

	if (Action->IsA<UCloseAction>())
		return DoorComponent->IsDoorOpen();

	if (Action->IsA<ULockAction>())
	{
		return !DoorComponent->IsDoorOpen();
	}

	if (Action->IsA<UUnlockAction>())
	{
		return false;
		//return DoorComponent->IsDoorOpen();
	}

	return true;
}

