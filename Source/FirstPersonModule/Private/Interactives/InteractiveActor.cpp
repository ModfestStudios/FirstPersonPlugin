// Fill out your copyright notice in the Description page of Project Settings.


#include "Interactives/InteractiveActor.h"

/*components*/
#include "Components/SceneComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Components/SkeletalMeshComponent.h"

// Sets default values
AInteractiveActor::AInteractiveActor(const FObjectInitializer& ObjectInitializer)
{
 	
	Mesh = ObjectInitializer.CreateDefaultSubobject<UStaticMeshComponent>(this, TEXT("MeshComponent"));
	if (Mesh)
	{
		SetRootComponent(Mesh);
	}

	InteractiveCollision = ObjectInitializer.CreateDefaultSubobject<UInteractiveCollisionComponent>(this, TEXT("InteractiveComponent"));
	if (InteractiveCollision)
	{
		InteractiveCollision->SetupAttachment(GetRootComponent());
	}

	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void AInteractiveActor::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AInteractiveActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AInteractiveActor::OnInteraction(AFirstPersonCharacter* User, UInteractiveCollisionComponent* InteractiveComponent, const UInteractiveAction* Action)
{
	Execute_BP_OnInteraction(this, User, InteractiveComponent, Action);
}

//void AInteractiveActor::OnInteraction_Implementation(AFirstPersonCharacter* User, UInteractiveCollisionComponent* InteractiveComponent, const UInteractiveAction* Action)
//{
//	
//	
//	/*by default simply just complete the interaction*/
//	if (InteractiveComponent)
//		InteractiveComponent->CompleteInteraction(User, Action);
//}

