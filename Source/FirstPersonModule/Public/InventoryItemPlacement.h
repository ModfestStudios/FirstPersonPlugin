// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "InventoryItemPlacement.generated.h"

UCLASS()
class FIRSTPERSONMODULE_API AInventoryItemPlacement : public AActor
{
	GENERATED_BODY()
public:

	UPROPERTY(VisibleAnywhere, Category = "Scene")
		class USceneComponent* SceneComponent;
	UPROPERTY(VisibleAnywhere, Category = "Components")
		class UArrowComponent* UpAxisComponent;


	//===========================================================================================================================================
	//=================================================================FUNCTIONS=================================================================
	//===========================================================================================================================================

	
public:	
	// Sets default values for this actor's properties
	AInventoryItemPlacement(const FObjectInitializer& ObjectInitializer);

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

};
