// Fill out your copyright notice in the Description page of Project Settings.


#include "OperatingSystem/Applications/Commands/TerminalCommand_Echo.h"
#include "OperatingSystem/Applications/TerminalApplication.h"

UTerminalCommand_Echo::UTerminalCommand_Echo()
{
	Command = "Echo";
}

FTerminalCommandResult UTerminalCommand_Echo::OnCommandExecuted(ATerminalApplication* Terminal, FTerminalCommandExecutionParameters CommandParameters)
{
	/*flag if our Blueprint version has overridden the function*/
	const bool bBlueprintImplementedFunction = GetClass()->IsFunctionImplementedInScript(GET_FUNCTION_NAME_CHECKED(UTerminalCommand, BP_OnCommandExecuted));

	FString EchoString;

	for (int32 i = 0; i < CommandParameters.Subcommands.Num(); i++)
	{
		if(i == 0)
			EchoString += CommandParameters.Subcommands[i].Subcommand.ToString();
		else
			EchoString += " " + CommandParameters.Subcommands[i].Subcommand.ToString();

	}

	if (bBlueprintImplementedFunction)
		return BP_OnCommandExecuted(Terminal, CommandParameters);
	else if (Terminal)
	{
		Terminal->PrintToTerminal("<ok>[ECHO]</> " + EchoString);
		return FTerminalCommandResult(true, "", false);
	}
	else
		return FTerminalCommandResult();
}
