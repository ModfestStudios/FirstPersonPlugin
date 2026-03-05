// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "InventoryItemSpawn.generated.h"

UCLASS()
class FIRSTPERSONMODULE_API AInventoryItemSpawn : public AActor
{
	GENERATED_BODY()
public:



	//=================================================================================================================================================
	//====================================================================FUNCTIONS====================================================================
	//=================================================================================================================================================
	
public:	
	// Sets default values for this actor's properties
	AInventoryItemSpawn();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

};
