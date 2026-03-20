// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Info.h"
#include "OperatingSystem.generated.h"

UENUM(BlueprintType)
enum class EOperatingSystemMainState : uint8
{
	PoweredOff,
	POST,
	BootLoader,
	LockScreen,
	Desktop
};

USTRUCT(BlueprintType)
struct FOperatingSystemVariable
{
	GENERATED_BODY()
public:
	
	UPROPERTY(EditAnywhere, Category = "System Variables")
		FName Name;
	UPROPERTY(EditAnywhere, Category = "System Variables")
		FName Value;
};

USTRUCT(BlueprintType)
struct FOperatingSystemPOST
{
	GENERATED_BODY()
public:

	UPROPERTY(EditAnywhere, Category = "Power-On Self Test")
		FText Message;
	UPROPERTY(EditAnywhere, Category = "Power-On Self Test")
		float ExecuteTime;
};


/**
 * 
 */
UCLASS(Blueprintable)
class FIRSTPERSONMODULE_API AOperatingSystem : public AInfo
{
	GENERATED_BODY()
public:

	/*core*/
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Operating System|Core")
		EOperatingSystemMainState SystemState;

	/*post*/
	UPROPERTY(EditAnywhere, Category = "Boot|Power-On Self Test")
		bool bEnablePOST;
	UPROPERTY(EditAnywhere, Category = "Boot|Power-On Self Test", meta = (DisplayName = "Power-On Self Test Messages"))
		TArray<FOperatingSystemPOST> PoweronSelfTestMessages;

	/*system variables*/
	UPROPERTY(EditAnywhere, Category = "Operating System|Variables")
		TArray<FOperatingSystemVariable> SystemVariables;




	/*operating system*/
	UPROPERTY(EditAnywhere, Category = "Operating System|Core")
		FText OperatingSystemName = NSLOCTEXT("OperatingSystem", "OSName", "Rabbit Hole");





	//=============================================================================================================================================================
	//==========================================================================FUNCTIONS==========================================================================
	//=============================================================================================================================================================

	AOperatingSystem();



	UFUNCTION(BlueprintCallable, Category = "Operating System|Boot Loader")
		virtual void Boot();
	
	UFUNCTION()
		virtual void BeginBootSequence();

	UFUNCTION()
		virtual void InitiateBIOS();



	UFUNCTION()
		virtual void EndBootSequence();
};
