// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Traps/TrapBase.h"
#include "LaserTrap.generated.h"

/**
 * 
 */
UCLASS()
class FIRSTPERSONMODULE_API ALaserTrap : public ATrap
{
	GENERATED_BODY()
public:


	UPROPERTY(EditAnywhere, Category = "Trap")
		float MaxLaserDistance = 5000.0f;

	
	UPROPERTY(VisibleAnywhere, Category = "Scene")
		class USceneComponent* SceneComponent;
	UPROPERTY(VisibleAnywhere, Category = "Rendering")
		class UArrowComponent* ArrowComponent;
	

	//=======================================================================================================================
	//=======================================================FUNCTIONS=======================================================
	//=======================================================================================================================

	ALaserTrap(const FObjectInitializer& ObjectInitializer);

#if WITH_EDITOR
	virtual void PostEditMove(bool bFinished) override;
#endif


	virtual void ActivateTrap() override;

	//=========LASER INITIATION=========

	UFUNCTION()
		FVector GetLaserEndpoint();
	UFUNCTION()
		virtual void UpdateLaserEndPoint();
	UFUNCTION()
		virtual void UpdateLaserCollision();
	UFUNCTION()
		virtual float GetLaserLength();
	UFUNCTION()
		virtual FVector GetLaserCenterPoint();



};
