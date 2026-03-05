// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "CounterClock.generated.h"

UCLASS(abstract)
class FIRSTPERSONMODULE_API ACounterClock : public AActor
{
	GENERATED_BODY()
public:

	UPROPERTY(VisibleAnywhere, Category = "Counter")
		class UCounterComponent* Counter;
	UPROPERTY(VisibleAnywhere, Category = "Counter")
		class UTextRenderComponent* TextRender;
	UPROPERTY(VisibleAnywhere, Category = "Rendering")
		class UStaticMeshComponent* Mesh;


		//===================================================================================================================================
		//=============================================================FUNCTIONS=============================================================
		//===================================================================================================================================
	
public:	
	// Sets default values for this actor's properties
	ACounterClock(const FObjectInitializer& ObjectInitializer);

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;


	UFUNCTION(BlueprintCallable, Category = "Counter")
		virtual void StartCounter();
	UFUNCTION(BlueprintCallable, Category = "Counter")
		virtual void StopCounter();
	UFUNCTION(BlueprintCallable, Category = "Counter")
		virtual void SetCountdownDuration(float NewDuration);

};
