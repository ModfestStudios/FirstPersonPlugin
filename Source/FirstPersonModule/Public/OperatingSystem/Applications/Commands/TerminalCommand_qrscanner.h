// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "OperatingSystem/TerminalCommand.h"
#include "TerminalCommand_qrscanner.generated.h"

/**
 * 
 */
UCLASS()
class FIRSTPERSONMODULE_API UTerminalCommand_qrscanner : public UTerminalCommand
{
	GENERATED_BODY()
public:
	virtual FTerminalCommandResult OnCommandExecuted(class ATerminalApplication* Terminal, FTerminalCommandExecutionParameters CommandParameters) override;

	UTerminalCommand_qrscanner();

private:
	FString getHelpText() const;
	FString listDevices() const;
	FString privateKeyShow() const;
};
