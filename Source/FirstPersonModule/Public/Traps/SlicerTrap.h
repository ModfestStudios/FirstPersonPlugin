// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Traps/TrapBase.h"
#include "SlicerTrap.generated.h"

/**
 * 
 */
UCLASS()
class FIRSTPERSONMODULE_API ASlicerTrap : public ATrap
{
	GENERATED_BODY()
public:

	UPROPERTY(VisibleAnywhere, Category = "Mesh")
		class USkeletalMeshComponent* Mesh;

	//===========================================================================================================================
	//=========================================================FUNCTIONS=========================================================
	//===========================================================================================================================
	ASlicerTrap(const FObjectInitializer& ObjectInitializer);
};
