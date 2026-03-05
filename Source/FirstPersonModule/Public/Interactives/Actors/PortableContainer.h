// Fill out your copyright notice in the Description page of Project Settings.

/*PortableContainer - A portable container is a container that holds a particular substance and can be picked up and moved around*/
#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "PortableContainer.generated.h"

UCLASS()
class FIRSTPERSONMODULE_API APortableContainer : public AActor
{
	GENERATED_BODY()
public:




	//=======================================================================================================================
	//=======================================================FUNCTIONS=======================================================
	//=======================================================================================================================

public:	
	// Sets default values for this actor's properties
	APortableContainer();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

};
