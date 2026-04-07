// Fill out your copyright notice in the Description page of Project Settings.


#include "OperatingSystem/Applications/Commands/TerminalCommand_Echo.h"
#include "OperatingSystem/Applications/TerminalApplication.h"

UTerminalCommand_Echo::UTerminalCommand_Echo()
{
	Command = "Echo";
}

FTerminalCommandResult UTerminalCommand_Echo::OnCommandExecuted(ATerminalApplication* Terminal, FTerminalCommandExecutionParameters CommandParameters)
{
	FString EchoString;

	for (int32 i = 0; i < CommandParameters.Subcommands.Num(); i++)
	{
		if(i == 0)
			EchoString += CommandParameters.Subcommands[i].Subcommand.ToString();
		else
			EchoString += " " + CommandParameters.Subcommands[i].Subcommand.ToString();

	}
	
	Terminal->PrintToTerminal("<ok>[ECHO]</> " + EchoString);
	return FTerminalCommandResult(true, "", false);
	
	
}
