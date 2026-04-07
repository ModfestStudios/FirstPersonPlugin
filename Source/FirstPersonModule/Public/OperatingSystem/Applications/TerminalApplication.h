// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "OperatingSystem/OperatingSystemApplication.h"
#include "OperatingSystem/TerminalCommand.h"
#include "TerminalApplication.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnTerminalUpdated);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(FOnTerminalCommandExecuted, TSubclassOf<UTerminalCommand>, Command, FTerminalCommandExecutionParameters, Parameters, FTerminalCommandResult, Results);



/**
 * 
 */


UENUM(BlueprintType)
enum class ETerminalMessageStyle : uint8
{
	None UMETA(DisplayName = "NONE"),
	OK UMETA(DisplayName = "OK"),
	Error UMETA(DisplayName = "ERROR"),
	Status UMETA(DisplayName = "STATUS")
};

UENUM(BlueprintType)
enum class ETerminalCommonMessage : uint8
{
	None UMETA(DisplayName = "None"),
	SyntaxLength UMETA(DisplayName = "Syntax Too Long/Short"),
	SyntaxFormat UMETA(DisplayName = "Syntax Format Error"),
	CommandNotFound UMETA(DisplayName = "Command Not Found"),
	UseHelp UMETA(DisplayName = "Use Help for Details")
};

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
public:
	UPROPERTY(BlueprintAssignable)
		FOnTerminalUpdated OnTerminalUpdated;
	UPROPERTY(BlueprintAssignable)
		FOnTerminalCommandExecuted OnTerminalCommandExecuted;

	//===============================================================================================================================================================
	//===========================================================================FUNCTIONS===========================================================================
	//===============================================================================================================================================================

public:
	ATerminalApplication();

	UFUNCTION(BlueprintCallable, Category = "Application|Terminal")
		virtual void ExecuteCommand(FString Command);
	UFUNCTION(BlueprintCallable, Category = "Application|Terminal")
		virtual bool IsValidCommand(FName Command, int32& CommandIndex);
	UFUNCTION(BlueprintCallable, Category = "Application|Terminal")
		virtual void ExecuteCommandObject(class TSubclassOf<UTerminalCommand> Command, FTerminalCommandExecutionParameters CommandParameters);
	UFUNCTION(BlueprintCallable, Category = "Application|Terminal")
		virtual void PrintToTerminal(const FString& Message, ETerminalMessageStyle Style = ETerminalMessageStyle::None);
	UFUNCTION(BlueprintCallable, Category= "Application|Terminal")
		virtual void PrintCommonTerminalResponse(ETerminalCommonMessage MessageResponse = ETerminalCommonMessage::None, FString UserDefinedValue1 = "");
	UFUNCTION(BlueprintCallable, Category = "Application|Terminal")
		virtual void ClearTerminal();
	UFUNCTION(BlueprintCallable, Category = "Application|Terminal")
		virtual void RefreshTerminal();

	UFUNCTION(BluePrintCallable)
		bool BlockedCommand(const FString& Command);

	
	
};
