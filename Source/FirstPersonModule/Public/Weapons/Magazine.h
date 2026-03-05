// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Magazine.generated.h"

UCLASS()
class FIRSTPERSONMODULE_API AMagazine : public AActor
{
	GENERATED_BODY()
public:

	UPROPERTY(EditDefaultsOnly, Category = "Configuration")
		class UProjectileInfo* Caliber;


	UPROPERTY(VisibleAnywhere, Category = "Rendering")
		class UStaticMeshComponent* Mesh;



	//===================================================================================================================
	//=====================================================FUNCTIONS=====================================================
	//===================================================================================================================
	
public:	
	// Sets default values for this actor's properties
	AMagazine(const FObjectInitializer& ObjectInitializer);

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

};
