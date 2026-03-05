// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "EquipmentLocker.generated.h"

UCLASS()
class FIRSTPERSONMODULE_API AEquipmentLocker : public AActor
{
	GENERATED_BODY()
public:


	UPROPERTY(EditAnywhere, Category = "Locker|UI")
		TSubclassOf<class ULoadoutWidget> LoadoutWidgetScreen;



	//=================================================================================================================
	//====================================================FUNCTIONS====================================================
	//=================================================================================================================
	
public:	
	// Sets default values for this actor's properties
	AEquipmentLocker();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;



	UFUNCTION(BlueprintCallable, Category = "Locker")
		virtual void OpenLockerWidget(class AFirstPersonPlayerController* Player);





};
