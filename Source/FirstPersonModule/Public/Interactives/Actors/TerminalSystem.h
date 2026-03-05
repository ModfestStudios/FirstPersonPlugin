// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Interactives/InteractiveActor.h"
#include "TerminalSystem.generated.h"

UCLASS()
class FIRSTPERSONMODULE_API ATerminalSystem : public AInteractiveActor
{
	GENERATED_BODY()
public:


	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Rendering")
		class UWidgetComponent* Widget;


	

	
public:	
	// Sets default values for this actor's properties
	ATerminalSystem(const FObjectInitializer& ObjectInitializer);

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

};
