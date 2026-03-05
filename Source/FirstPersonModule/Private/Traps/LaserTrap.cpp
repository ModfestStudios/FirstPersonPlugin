// Fill out your copyright notice in the Description page of Project Settings.


#include "Traps/LaserTrap.h"
#include "Components/ArrowComponent.h"
#include "NiagaraComponent.h"
#include "NiagaraFunctionLibrary.h"
#include "Components/BoxComponent.h"

/*engine*/
#include "Engine/World.h"


ALaserTrap::ALaserTrap(const FObjectInitializer& ObjectInitializer)
{
	bUseDamageRate = true;

	SceneComponent = ObjectInitializer.CreateDefaultSubobject<USceneComponent>(this, TEXT("SceneComp"));
	if (SceneComponent)
	{
		SetRootComponent(SceneComponent);
	}

	ArrowComponent = ObjectInitializer.CreateDefaultSubobject<UArrowComponent>(this, TEXT("ArrowComp"));
	if (ArrowComponent)
	{
		ArrowComponent->SetupAttachment(GetRootComponent());
	}
}

#if WITH_EDITOR
void ALaserTrap::PostEditMove(bool bFinished)
{
	Super::PostEditMove(bFinished);

	if (bFinished)
	{
		UpdateLaserEndPoint();
		UpdateLaserCollision();
	}
}

#endif

void ALaserTrap::ActivateTrap()
{
	Super::ActivateTrap();


}


//==================================
//=========LASER INITIATION=========
//==================================

FVector ALaserTrap::GetLaserEndpoint()
{
	FVector ForwardVector = GetActorForwardVector();
	FVector TraceStart = GetActorLocation();
	FVector TraceEnd = TraceStart + (ForwardVector * MaxLaserDistance);
	FVector TraceResults = TraceEnd;
	FHitResult HitResults;

	if (GetWorld()->LineTraceSingleByChannel(HitResults, TraceStart, TraceEnd, ECollisionChannel::ECC_WorldDynamic))
	{
		//DrawDebugSphere(GetWorld(), HitResults.Location, 12, 4, FColor::Purple, false, 2.5f);
		TraceResults = HitResults.Location;
	}	

	return TraceResults;
}

void ALaserTrap::UpdateLaserEndPoint()
{
	TArray<UNiagaraComponent*> Lasers;
	GetComponents<UNiagaraComponent>(Lasers);
	FVector LaserEndpoint = GetLaserEndpoint();	

	for (UNiagaraComponent* Laser : Lasers)
	{
		if (Laser)
		{
			// Get the inverse of the component's world transform
			FTransform ComponentToWorldInverse = Laser->GetComponentTransform().Inverse();

			// Convert LaserEndpoint from world space to component's local space
			FVector LocalPosition = ComponentToWorldInverse.TransformPosition(LaserEndpoint);

			// Set the local position for the Niagara component
			Laser->SetNiagaraVariableVec3(FString("Beam End"), LocalPosition);
		}
	}	
}

void ALaserTrap::UpdateLaserCollision()
{
	TArray<UBoxComponent*> CollisionComponents;
	GetComponents<UBoxComponent>(CollisionComponents);
	float LaserLength = GetLaserLength();
	FVector CenterPoint = GetLaserCenterPoint();

	for (UBoxComponent* Box : CollisionComponents)
	{
		/*adjust collision*/
		FVector NewLocation = Box->GetRelativeLocation();
		NewLocation.X = CenterPoint.X;
		Box->SetRelativeLocation(NewLocation);

		FVector NewScale = Box->GetRelativeScale3D();
		NewScale.X = LaserLength / 2.0f;
		Box->SetRelativeScale3D(NewScale);
	}
}

float ALaserTrap::GetLaserLength()
{
	FVector LaserEndpoint = GetLaserEndpoint();
	FVector BaseLocation = GetActorLocation();
	float Length = FVector::Dist(BaseLocation, LaserEndpoint);

	return Length;
}

FVector ALaserTrap::GetLaserCenterPoint()
{
	// Get the world transform of the actor or component (you can choose the component you're interested in)
	FTransform ComponentTransform = GetRootComponent()->GetComponentTransform();

	// Get the world location of the actor and laser endpoint
	FVector ActorWorldLocation = GetActorLocation();
	FVector LaserEndpointWorldLocation = GetLaserEndpoint();

	// Convert the actor location and laser endpoint into local space
	FVector ActorLocalLocation = ComponentTransform.InverseTransformPosition(ActorWorldLocation);
	FVector LaserEndpointLocalLocation = ComponentTransform.InverseTransformPosition(LaserEndpointWorldLocation);

	// Calculate the center in local space
	FVector LaserCenterLocal = (ActorLocalLocation + LaserEndpointLocalLocation) / 2.0f;

	return LaserCenterLocal;
}
