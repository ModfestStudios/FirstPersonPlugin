// Fill out your copyright notice in the Description page of Project Settings.


#include "Interactives/InteractiveActor.h"

#include "Interactives/Interactives.h"

#include "Players/FirstPersonPlayerController.h"
#include "Characters/FirstPersonCharacter.h"

/*components*/
#include "Components/InteractiveComponent.h"
#include "Components/SceneComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "Components/InventoryManagerComponent.h"


/*network*/
#include "Net/UnrealNetwork.h"

void AInteractiveActor::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AInteractiveActor, ActiveUsers);
}

// Sets default values
AInteractiveActor::AInteractiveActor(const FObjectInitializer& ObjectInitializer)
{ 	
	Mesh = ObjectInitializer.CreateDefaultSubobject<UStaticMeshComponent>(this, TEXT("MeshComponent"));
	if (Mesh)
	{
		SetRootComponent(Mesh);
	}

	InteractiveComponent = ObjectInitializer.CreateDefaultSubobject<UInteractiveComponent>(this, TEXT("Interactive Component"));
	if (InteractiveComponent)
	{
		InteractiveComponent->SetupAttachment(GetRootComponent());
	}

	bReplicates = true;
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void AInteractiveActor::BeginPlay()
{
	Super::BeginPlay();


	/*begin bindings*/
	if (InteractiveComponent)
	{
		InteractiveComponent->OnInteractionBegin.AddDynamic(this,&AInteractiveActor::NativeOnInteractionBegins);
		InteractiveComponent->OnInteractionEnd.AddDynamic(this, &AInteractiveActor::NativeOnInteractionEnds);
	}
	
}

// Called every frame
void AInteractiveActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void AInteractiveActor::NativeOnInteractionBegins(AFirstPersonCharacter* User, const UInteractiveAction* Action)
{
	


}

void AInteractiveActor::NativeOnInteractionEnds(AFirstPersonCharacter* User, const UInteractiveAction* Action, EInteractionEndReason EndReason)
{
	
}

//void AInteractiveActor::OnInteraction(AFirstPersonCharacter* User, UInteractiveCollisionComponent* InteractiveComponent, const UInteractiveAction* Action)
//{
//	if (!ActiveUsers.Contains(User))
//		ActiveUsers.Add(User);
//
//	Execute_BP_OnInteraction(this, User, InteractiveComponent, Action);
//}
//
//void AInteractiveActor::OnInteractionEnds(AFirstPersonCharacter* User, UInteractiveCollisionComponent* InteractiveComponent, const UInteractiveAction* Action)
//{
//
//}

bool AInteractiveActor::IsBeingInteractedBy(AFirstPersonCharacter* User)
{
	return ActiveUsers.Contains(User);
}

//void AInteractiveActor::OnInteraction_Implementation(AFirstPersonCharacter* User, UInteractiveCollisionComponent* InteractiveComponent, const UInteractiveAction* Action)
//{
//	
//	
//	/*by default simply just complete the interaction*/
//	if (InteractiveComponent)
//		InteractiveComponent->CompleteInteraction(User, Action);
//}

