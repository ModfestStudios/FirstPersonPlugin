// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "OperatingSystem/TerminalCommand.h"
#include "TerminalCommand_rm.generated.h"

/**
 * 
 */
UCLASS()
class FIRSTPERSONMODULE_API UTerminalCommand_rm : public UTerminalCommand
{
	GENERATED_BODY()
public:
	virtual FTerminalCommandResult OnCommandExecuted(class ATerminalApplication* Terminal, FTerminalCommandExecutionParameters CommandParameters) override;

	UTerminalCommand_rm();
	
};
