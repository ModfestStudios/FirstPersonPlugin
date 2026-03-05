// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Interfaces/InteractiveActorInterface.h"
#include "Door.generated.h"

UCLASS(abstract)
class FIRSTPERSONMODULE_API ADoor : public AActor, public IInteractiveActorInterface
{
	GENERATED_BODY()
public:
	

	/*auto-initialized list of doors for this actor*/
	TArray<class UDoorComponent*> DoorComponents;

	//===============================================================================================================
	//===================================================FUNCTIONS===================================================
	//===============================================================================================================
	
public:	
	// Sets default values for this actor's properties
	ADoor(const FObjectInitializer& ObjectInitializer);

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;


public:
	UFUNCTION(BlueprintCallable, Category = "Interactive|Door")
		virtual void Open();
	UFUNCTION(BlueprintCallable, Category = "Interactive|Door")
		virtual void Close();


};
