// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Inventory.h"
#include "GameFramework/Actor.h"
#include "Interfaces/InventoryItemInterface.h"
#include "InventoryItem.generated.h"


UCLASS(abstract)
class FIRSTPERSONMODULE_API AInventoryItem : public AActor, public IInventoryItemInterface
{
	GENERATED_BODY()
public:



	//=====================================================================================================================================
	//==============================================================FUNCTIONS==============================================================
	//=====================================================================================================================================

	virtual void OnEquip_Implementation() override;
	virtual void OnUnequip_Implementation() override;
	virtual void OnPickedUp_Implementation() override;


public:	
	// Sets default values for this actor's properties
	AInventoryItem(const FObjectInitializer& ObjectInitializer);

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

};
