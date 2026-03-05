// Fill out your copyright notice in the Description page of Project Settings.


#include "Interactives/Actors/Elevator.h"
#include "Components/ElevatorDestinationComponent.h"


AElevator::AElevator(const FObjectInitializer& ObjectInitializer)
	:Super(ObjectInitializer)
{
	SceneComponent = ObjectInitializer.CreateDefaultSubobject<USceneComponent>(this, TEXT("ElevatorSceneComponent"));
	SetRootComponent(SceneComponent);

	if (Mesh)
	{
		Mesh->SetupAttachment(GetRootComponent());
		Mesh->SetVisibility(false, false);
	}

	LeftDoor = ObjectInitializer.CreateDefaultSubobject<UStaticMeshComponent>(this, TEXT("LeftDoorComponent"));
	RightDoor = ObjectInitializer.CreateDefaultSubobject<UStaticMeshComponent>(this, TEXT("RightDoorComponent"));

	if(LeftDoor)
	{
		LeftDoor->SetupAttachment(Mesh);
	}
	if (RightDoor)
	{
		RightDoor->SetupAttachment(Mesh);
	}

	DestinationsComponent = ObjectInitializer.CreateDefaultSubobject<USceneComponent>(this, TEXT("DestinationsComponent"));
	if (DestinationsComponent)
	{
		DestinationsComponent->SetupAttachment(GetRootComponent());
	}
	StartingDestinationComponent = ObjectInitializer.CreateDefaultSubobject<UElevatorDestinationComponent>(this, TEXT("Destination_G"));
	if (StartingDestinationComponent)
	{
		StartingDestinationComponent->SetupAttachment(DestinationsComponent);
		StartingDestinationComponent->DestinationId = "G";
	}
}

#if WITH_EDITOR
void AElevator::OnConstruction(const FTransform& Transform)
{
	Super::OnConstruction(Transform);
	

	//TArray<FElevatorDestination> OldDestinations = Destinations;
	//Destinations.Empty();
	//Destinations.Add(FElevatorDestination(DefaultDestinationId, GetActorLocation(),OldDestinations.IsValidIndex(0) ? OldDestinations[0].Name : NAME_None));
	//SetDestination(Destinations[0].DestinationId);
	//
	//TArray<UElevatorDestinationComponent*> DestinationComponents;
	//GetComponents<UElevatorDestinationComponent>(DestinationComponents);

	//for(UElevatorDestinationComponent* DestinationComp : DestinationComponents)
	//{
	//	int32 i = Destinations.Add(FElevatorDestination(DestinationComp->DestinationId, DestinationComp->GetRelativeLocation()));
	//	int32 j = OldDestinations.IndexOfByPredicate([&](const FElevatorDestination& Original) { return Original.DestinationId == Destinations[i].DestinationId; });
	//	if(j != INDEX_NONE)
	//		Destinations[i].Name = OldDestinations[j].Name;
	//}

	//Modify();
	//if (UPackage* Pkg = GetOutermost())
	//	Pkg->SetDirtyFlag(true);
}
#endif

void AElevator::BeginPlay()
{
	Super::BeginPlay();

	/*init*/
	Destinations.Empty(); //cleanup for safety
	/*set starting point*/

	TInlineComponentArray <UElevatorDestinationComponent*> DestinationComps;
	GetComponents<UElevatorDestinationComponent>(DestinationComps);

	/*add all components to Destinations Array*/
	for (UElevatorDestinationComponent* DestinationComp : DestinationComps)
	{
		Destinations.Add(FElevatorDestination(DestinationComp->DestinationId, DestinationComp->GetRelativeLocation()));
	}
		
	
	SetDestination(StartingDestination);
	SnapToDestination(StartingDestination);

	if (Mesh)
		Mesh->SetVisibility(true);
}

void AElevator::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	

	if (bElevatorMoving)
	{	
		/*init*/
		float MovementThisTick = ElevatorSpeed * DeltaTime;
		int32 i = Destinations.IndexOfByPredicate([&](const FElevatorDestination x) { return x.DestinationId == NextDestination; });
		FVector CurrentLoc = Mesh->GetComponentLocation();
		FVector TargetLoc = Destinations.IsValidIndex(i) ? (GetActorLocation() + Destinations[i].Location) : CurrentLoc;
		FVector Direction = (TargetLoc - CurrentLoc).GetSafeNormal();
		FVector NewMovement = CurrentLoc + (Direction * MovementThisTick);

		/*movement update*/
		Mesh->SetWorldLocation(NewMovement);

		/*success check*/		
		if (FVector::Distance(CurrentLoc, TargetLoc) <= StoppingThreshold)
		{
			Mesh->SetWorldLocation(TargetLoc); //make sure to snap for accuracy
			CurrentDestination = NextDestination;
			NextDestination = NAME_None;
			StopElevator();
		}	
	}
}

void AElevator::OnDestinationComponentAdded(UElevatorDestinationComponent* Destination)
{

}


void AElevator::StartElevator()
{
	CurrentDestination = NAME_None;	
	bElevatorMoving = true;
}

void AElevator::StopElevator()
{
	bElevatorMoving = false;
}

void AElevator::SnapToDestination(FName Destination)
{
	for (const FElevatorDestination& Dest : Destinations)
	{
		if (Dest.DestinationId == Destination)
		{
			SetDestination(Dest.DestinationId);
			Mesh->SetRelativeLocation(Dest.Location);
			return;
		}
	}
	
}

void AElevator::SetDestination(FName Destination)
{
	NextDestination = Destination;
}