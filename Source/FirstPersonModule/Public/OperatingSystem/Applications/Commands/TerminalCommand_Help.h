// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "OperatingSystem/TerminalCommand.h"
#include "TerminalCommand_Help.generated.h"

/**
 * 
 */
UCLASS()
class FIRSTPERSONMODULE_API UTerminalCommand_Help : public UTerminalCommand
{
	GENERATED_BODY()
public:
	virtual FTerminalCommandResult OnCommandExecuted(class ATerminalApplication* Terminal, FTerminalCommandExecutionParameters CommandParameters) override;

	UTerminalCommand_Help();
	
private:
	FString getHelpText() const;
	FString getCommandsHelpText() const;
	FString getAboutHelpText() const;
};
