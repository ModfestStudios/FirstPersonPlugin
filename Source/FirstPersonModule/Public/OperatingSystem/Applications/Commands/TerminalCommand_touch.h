// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "OperatingSystem/TerminalCommand.h"
#include "TerminalCommand_touch.generated.h"

/**
 * 
 */
UCLASS()
class FIRSTPERSONMODULE_API UTerminalCommand_touch : public UTerminalCommand
{
	GENERATED_BODY()
public:
	virtual FTerminalCommandResult OnCommandExecuted(class ATerminalApplication* Terminal, FTerminalCommandExecutionParameters CommandParameters) override;

	UTerminalCommand_touch();
	
};
