// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Gib.generated.h"

UCLASS()
class FIRSTPERSONMODULE_API AGib : public AActor
{
	GENERATED_BODY()
public:
	
	UPROPERTY()
		class USceneComponent* SceneComponent;
	UPROPERTY()
		class UStaticMeshComponent* StaticGib;
	UPROPERTY()
		class USkeletalMeshComponent* SkeletalGib;


	//===============================================================================================================================
	//===========================================================FUNCTIONS===========================================================
	//===============================================================================================================================

public:	
	// Sets default values for this actor's properties
	AGib(const FObjectInitializer& ObjectInitializer);

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UFUNCTION()
		virtual void InitializeStaticGib(UStaticMesh* StaticMesh);
	UFUNCTION()
		virtual void InitializeSkeletalGib(USkeletalMesh* SkeletalMesh);
	UFUNCTION()
		virtual void ApplyImpulse(FVector Impulse);

};
