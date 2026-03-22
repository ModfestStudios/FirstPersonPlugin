// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "OperatingSystem/OperatingSystemApplication.h"
#include "OperatingSystem/TerminalCommand.h"
#include "TerminalApplication.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnTerminalUpdated);


/**
 * 
 */
UCLASS()
class FIRSTPERSONMODULE_API ATerminalApplication : public AOperatingSystemApplication
{
	GENERATED_BODY()
public:


	/*list of messages currently on-screen by the terminal*/
	UPROPERTY(BlueprintReadOnly, Category = "Application|Terminal")
		TArray<FString> TerminalMessages;
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Application|Terminal")
		TArray<TSubclassOf<class UTerminalCommand>> Commands;


	/*events*/
	UPROPERTY(BlueprintAssignable)
		FOnTerminalUpdated OnTerminalUpdated;

	//===============================================================================================================================================================
	//===========================================================================FUNCTIONS===========================================================================
	//===============================================================================================================================================================
	
	ATerminalApplication();

	UFUNCTION(BlueprintCallable, Category = "Application|Terminal")
		virtual void ExecuteCommand(FString Command);
	UFUNCTION(BlueprintCallable, Category = "Application|Terminal")
		virtual bool IsValidCommand(FName Command, int32& CommandIndex);
	
	
	UFUNCTION(BlueprintCallable, Category = "Application|Terminal")
		virtual void ExecuteCommandObject(class TSubclassOf<UTerminalCommand> Command, FTerminalCommandExecutionParameters CommandParameters);

	
	UFUNCTION(BlueprintCallable, Category = "Application|Terminal")
		virtual void PrintToTerminal(const FString& Message);
	UFUNCTION(BlueprintCallable, Category = "Application|Terminal")
		virtual void ClearTerminal();

	UFUNCTION(BlueprintCallable, Category = "Application|Terminal")
		virtual void RefreshTerminal();





	
	
};
