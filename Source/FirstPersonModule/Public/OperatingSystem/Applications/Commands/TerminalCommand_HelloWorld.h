// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "OperatingSystem/TerminalCommand.h"
#include "TerminalCommand_HelloWorld.generated.h"

/**
 * 
 */
UCLASS()
class FIRSTPERSONMODULE_API UTerminalCommand_HelloWorld : public UTerminalCommand
{
	GENERATED_BODY()
public:
	virtual FTerminalCommandResult OnCommandExecuted(class ATerminalApplication* Terminal, FTerminalCommandExecutionParameters CommandParameters) override;

	UTerminalCommand_HelloWorld();

private:
	FString showEncryptedMessage() const;
	FString showDecryptedMessage() const;
};
