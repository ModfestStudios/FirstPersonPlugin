// Fill out your copyright notice in the Description page of Project Settings.


#include "Characters/CharacterPreviewPawn.h"

/*components*/
#include "Components/SkeletalMeshComponent.h"

// Sets default values
ACharacterPreviewPawn::ACharacterPreviewPawn(const FObjectInitializer& ObjectInitializer)
{
 	
	Mesh = ObjectInitializer.CreateDefaultSubobject<USkeletalMeshComponent>(this, TEXT("Mesh"));
	if (Mesh)
	{
		SetRootComponent(Mesh);
	}

	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	bReplicates = false;
	bOnlyRelevantToOwner = false;
}

// Called when the game starts or when spawned
void ACharacterPreviewPawn::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ACharacterPreviewPawn::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

