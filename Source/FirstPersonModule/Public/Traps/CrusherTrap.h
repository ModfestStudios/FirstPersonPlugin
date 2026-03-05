// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Traps/TrapBase.h"
#include "CrusherTrap.generated.h"

/**
 * 
 */
UCLASS(abstract)
class FIRSTPERSONMODULE_API ACrusherTrap : public ATrap
{
	GENERATED_BODY()
public:

	UPROPERTY(VisibleAnywhere, Category = "Rendering")
		class USkeletalMeshComponent* Mesh;


	//===============================================================================================================================
	//===========================================================FUNCTIONS===========================================================
	//===============================================================================================================================
	ACrusherTrap(const FObjectInitializer& ObjectInitializer);
	
};
