// Fill out your copyright notice in the Description page of Project Settings.


#include "Traps/SawBladeTrap.h"
#include "GameFramework/Character.h"
#include "Components/BoxComponent.h"
#include "Components/SceneComponent.h"
#include "Components/SkeletalMeshComponent.h"

ASawBladeTrap::ASawBladeTrap(const FObjectInitializer& ObjectInitializer)
{
	bAutoActivate = true;
	bAutoReset = false;
	DamageAmount = 32.0f;

	Mesh = ObjectInitializer.CreateDefaultSubobject<USkeletalMeshComponent>(this, TEXT("MeshComponent"));
	if (Mesh)
	{
		SetRootComponent(Mesh);
		Mesh->SetCollisionEnabled(ECollisionEnabled::PhysicsOnly);
		Mesh->SetCollisionProfileName("BlockAllDynamic");
		Mesh->CanCharacterStepUpOn = ECanBeCharacterBase::ECB_No;		
	}
	CollisionComponent = ObjectInitializer.CreateDefaultSubobject<UBoxComponent>(this, TEXT("CollisionComponent"));
	if (CollisionComponent)
	{
		CollisionComponent->SetupAttachment(Mesh,BladeAttachmentSocketName);		
		CollisionComponent->SetBoxExtent(FVector(150, 5, 150));
		CollisionComponent->SetGenerateOverlapEvents(true);
		CollisionComponent->SetCollisionProfileName("OverlapAllDynamic");
	}
}

void ASawBladeTrap::BeginPlay()
{	
	Super::BeginPlay();
	if (CollisionComponent)
	{
		CollisionComponent->OnComponentBeginOverlap.AddDynamic(this, &ASawBladeTrap::OnBladeOverlap);
		CollisionComponent->OnComponentEndOverlap.AddDynamic(this, &ASawBladeTrap::OnBladeEndOverlap);
	}
}

void ASawBladeTrap::OnBladeOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{	
	/*server logic*/
	if (GetNetMode() < NM_Client)
	{
		if (OtherActor)
			DamageActor(OtherActor);

		if (bTrapActivated)
		{
			/*if it's a character type*/
			if (ACharacter* Character = Cast<ACharacter>(OtherActor))
			{
				PushCharacterBack(Character, KnockbackStrength, KnockbackUpwardStrength);
			}
		}
	}

	/*call blueprint version*/
	BP_OnBladeOverlap(OverlappedComponent, OtherActor, OtherComp, OtherBodyIndex, bFromSweep, SweepResult);
}

void ASawBladeTrap::OnBladeEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{

	/*call blueprint version*/
	BP_OnBladeEndOverlap(OverlappedComponent, OtherActor, OtherComp, OtherBodyIndex);
}
