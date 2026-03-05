// Fill out your copyright notice in the Description page of Project Settings.


#include "Subsystems/GoreSubsystem.h"
#include "Kismet/GameplayStatics.h"
#include "Components/DecalComponent.h"
#include "Gore/Gib.h"

/*engine*/
#include "Engine/World.h"

void UGoreSubsystem::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void UGoreSubsystem::SpawnStaticGib(UStaticMesh* Mesh, FVector Location, FVector Impulse, float Scale)
{
	if (UWorld* World = GetWorld())
	{
		FActorSpawnParameters SpawnParams;
		SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn; //we always want meaty gibs
		FRotator RandomRot = FRotator(
			FMath::RandRange(-180, 180),
			FMath::RandRange(-180, 180),
			FMath::RandRange(-180, 180)
		);
		
		AGib* NewMeatyGib = World->SpawnActor<AGib>(Location, RandomRot, SpawnParams);

		if (NewMeatyGib)
		{			
			NewMeatyGib->InitializeStaticGib(Mesh);
			NewMeatyGib->SetActorRelativeScale3D(FVector(Scale));
			NewMeatyGib->ApplyImpulse(Impulse);

			ActiveGibs.Add(NewMeatyGib);
		}
	}		
}

void UGoreSubsystem::SpawnSkeletalGib(USkeletalMesh* Mesh, FVector Location, FVector Impulse, float Scale)
{

}

FGoreDecal UGoreSubsystem::SpawnGoreDecal(class UMaterialInterface* GoreMaterial, FVector Location, FVector SurfaceNormal,float Size, bool bPermanentDecal, float Lifetime)
{
	UDecalComponent* Decal = UGameplayStatics::SpawnDecalAtLocation(GetWorld(), GoreMaterial, FVector(DefaultDecalHeight,Size,Size), Location, SurfaceNormal.ToOrientationRotator());

	FGoreDecal GoreDecal;
	GoreDecal.Decal = Decal;
	GoreDecal.bPermanentDecal = bPermanentDecal;
	GoreDecal.Lifetime = Lifetime;

	ActiveDecals.Add(GoreDecal);

	return GoreDecal;
}
