// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "TerminalCommand.generated.h"

USTRUCT(BlueprintType)
struct FTerminalCommandResult
{
	GENERATED_BODY()
public:

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
		bool bSuccess = false;
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
		FString TerminalMessage;
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
		bool bClearScreen = false;

	FTerminalCommandResult()
	{
		bSuccess = false;
		TerminalMessage = "...";
		bClearScreen = false;
	};

	FTerminalCommandResult(bool bSuccess, FString TerminalMessage, bool bClearScreen)
	{
		this->bSuccess = bSuccess;
		this->TerminalMessage = TerminalMessage;
		this->bClearScreen = bClearScreen;
	}

};

USTRUCT(BlueprintType)
struct FTerminalSubcommand
{
	GENERATED_BODY()
public:
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "Subcommand")
		FName Subcommand;
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "Subcommand")
		FString SubcommandHint;

	FTerminalSubcommand() {};
	FTerminalSubcommand(FName Subcommand)
	{
		this->Subcommand = Subcommand;
	}

};


/*command execution paramaters*/
USTRUCT(BlueprintType)
struct FTerminalCommandExecutionParameters
{
	GENERATED_BODY()
public:

	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Terminal Command Execution Parameters")
		FName Command;
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Terminal Command Execution Parameters")
		TArray<FTerminalSubcommand> Subcommands;
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Terminal Command Execution Parameters")
		TArray<FString> Flags;

};

/**
 * 
 */
UCLASS(Blueprintable)
class FIRSTPERSONMODULE_API UTerminalCommand : public UObject
{
	GENERATED_BODY()
public:

	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "Terminal Command")
		FName Command;
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "Terminal Command")
		FString CommandHint;
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "Terminal Command")
		TArray<FTerminalSubcommand> Subcommands;
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "Terminal Command")
		TArray<FString> CommandFlags;

	UPROPERTY(EditDefaultsOnly, Category = "Terminal Command")
		bool bRequireElevatedPermissions = false;


	//===============================================================================================================================================================
	//===========================================================================FUNCTIONS===========================================================================
	//===============================================================================================================================================================

	UFUNCTION()
		virtual FTerminalCommandResult OnCommandExecuted(class ATerminalApplication* Terminal, FTerminalCommandExecutionParameters CommandParameters);
	UFUNCTION(BlueprintImplementableEvent, Category = "Terminal Command", meta = (DisplayName = "On Command Executed"))
		FTerminalCommandResult BP_OnCommandExecuted(class ATerminalApplication* Terminal, FTerminalCommandExecutionParameters CommandParameters);


	
};
