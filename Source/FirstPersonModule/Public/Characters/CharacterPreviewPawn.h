// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "CharacterPreviewPawn.generated.h"

UCLASS()
class FIRSTPERSONMODULE_API ACharacterPreviewPawn : public AActor
{
	GENERATED_BODY()
public:


	UPROPERTY(VisibleAnywhere, Category = "Mesh")
		USkeletalMeshComponent* Mesh;
	
public:	
	// Sets default values for this actor's properties
	ACharacterPreviewPawn(const FObjectInitializer& ObjectInitializer);

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

};
