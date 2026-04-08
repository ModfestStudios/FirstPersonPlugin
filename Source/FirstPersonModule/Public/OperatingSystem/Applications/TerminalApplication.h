// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "OperatingSystem/OperatingSystemApplication.h"
#include "OperatingSystem/TerminalCommand.h"
#include "TerminalApplication.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnTerminalUpdated);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(FOnTerminalCommandExecuted, TSubclassOf<UTerminalCommand>, Command, FTerminalCommandExecutionParameters, Parameters, FTerminalCommandResult, Results);




USTRUCT(BlueprintType)
struct FDelayedTerminalMessage
{
	GENERATED_BODY()
public:
	UPROPERTY(BlueprintReadOnly, Category = "Terminal Message")
		FString Message;
	UPROPERTY(BlueprintReadOnly, Category = "Terminal Message")
		float Delay;
	UPROPERTY(BlueprintReadOnly, Category = "Terminal Message")
		float DelayRemaining;

	FDelayedTerminalMessage() 
	{
		Message = "No Message Supplied";
		Delay = 0.0f;
		DelayRemaining = 0.0f;
	}

	FDelayedTerminalMessage(FString MessageToPrint, float DelayTime)
	{
		Message = MessageToPrint;
		Delay = DelayTime;
		DelayRemaining = DelayTime;
	}
};
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
public:
	UPROPERTY(BlueprintReadOnly, Category = "Application|Terminal")
		TArray<FString> TerminalMessages;
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Application|Terminal")
		TArray<TSubclassOf<class UTerminalCommand>> Commands;
	
	/*commands waiting to be executed*/
	UPROPERTY(BlueprintReadOnly, Category = "Application|Terminal")
		TArray<FDelayedTerminalMessage> MessageQueue;

protected:
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Application|Terminal")
		TArray<FString> CommandHistory;
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Application|Terminal")
		int32 CommandHistoryIndex = -1; //-1 == no display - user is typing in blank state



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

	virtual void Tick(float DeltaTime) override;

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Application|Terminal")
		bool IsMessageQueued();

protected:
	virtual void ProcessDelayedMessages(float DeltaTime);


public:
	UFUNCTION(BlueprintCallable, Category = "Application|Terminal")
		virtual void ExecuteCommand(FString Command);
	UFUNCTION(BlueprintCallable, Category = "Application|Terminal")
		virtual bool IsValidCommand(FName Command, int32& CommandIndex);
	UFUNCTION(BlueprintCallable, Category = "Application|Terminal")
		virtual void ExecuteCommandObject(class TSubclassOf<UTerminalCommand> Command, FTerminalCommandExecutionParameters CommandParameters);
	UFUNCTION(BlueprintCallable, Category = "Application|Terminal")
		virtual void PrintToTerminal(const FString& Message, ETerminalMessageStyle Style = ETerminalMessageStyle::None,float Delay = 0.0f);
	UFUNCTION(BlueprintCallable, Category= "Application|Terminal")
		virtual void PrintCommonTerminalResponse(ETerminalCommonMessage MessageResponse = ETerminalCommonMessage::None, FString UserDefinedValue1 = "");
private:
	/*the function that performs the actual print (called by all public facing PrintToTerminal commands)*/
	UFUNCTION()
		virtual void PrintMessage(FString Message);
public:
	UFUNCTION(BlueprintCallable, Category = "Application|Terminal")
		virtual void ClearTerminal();
	UFUNCTION(BlueprintCallable, Category = "Application|Terminal")
		virtual void RefreshTerminal();
	
	//=====================
	//=======PARSING=======
	//=====================
	
	/*combines quotes into a single entry*/
	UFUNCTION()
		virtual void ParseQuotes(TArray<FString>& CommandArray);
	/*takes the command array and parses out all the commands, subcommands, and flags and updates CommandParamaters with results*/
	UFUNCTION()
		virtual void ParseCommands(TArray<FString>& CommandArray, FTerminalCommandExecutionParameters& CommandParameters);
	
	//=====================
	//=======HISTORY=======
	//=====================

	UFUNCTION(BlueprintCallable, Category = "Application|Terminal")
		virtual void AddCommandToHistory(FString Command);
	UFUNCTION(BlueprintCallable, Category = "Application|Terminal")
		virtual void ResetCommandHistory();
	/*returns the previous command in order*/
	UFUNCTION(BlueprintPure, Category = "Application|Terminal")
		virtual FString GetPreviousCommand();
	UFUNCTION(BlueprintPure, Category = "Application|Terminal")
		virtual FString GetNextCommand();
	/*clears all history*/
	UFUNCTION(BlueprintCallable, Category = "Application|Terminal")
		virtual void ClearCommandHistory();

	
	UFUNCTION(BluePrintCallable)
		bool BlockedCommand(const FString& Command);

	
	
};
