// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Info.h"
#include "MissionInfo.generated.h"

/**
 * 
 */
UCLASS()
class FIRSTPERSONMODULE_API AMissionInfo : public AInfo
{
	GENERATED_BODY()
public:

	UPROPERTY(BlueprintReadOnly, Category = "Mission")
		FText MissionName;



	//===============================================================================================================
	//===================================================FUNCTIONS===================================================
	//===============================================================================================================
	
	AMissionInfo();
};