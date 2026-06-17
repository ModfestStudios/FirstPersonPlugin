// Fill out your copyright notice in the Description page of Project Settings.


#include "OperatingSystem/Applications/Commands/TerminalCommand_Echo.h"
#include "OperatingSystem/Applications/TerminalApplication.h"

UTerminalCommand_Echo::UTerminalCommand_Echo()
{
	Command = "Echo";

	CommandFlags.Add(FTerminalCommandFlag("delay"));
}

FTerminalCommandResult UTerminalCommand_Echo::OnCommandExecuted(ATerminalApplication* Terminal, FTerminalCommandExecutionParameters CommandParameters)
{
	FString EchoString;
	float MessageDelay = 0.0f;

	for (int32 i = 0; i < CommandParameters.Subcommands.Num(); i++)
	{
		if(i == 0)
			EchoString += CommandParameters.Subcommands[i].Subcommand.ToString();
		else
			EchoString += " " + CommandParameters.Subcommands[i].Subcommand.ToString();
	}

	/*flags*/
	for (FTerminalCommandFlag& Flag : CommandParameters.Flags)
	{
		if (Flag == "delay" && Flag.Value.IsNumeric())
		{
			MessageDelay = FCString::Atof(*Flag.Value); //convert to decimal
		}

	}
	
	Terminal->PrintToTerminal(EchoString, ETerminalMessageStyle::OK, MessageDelay);
	return FTerminalCommandResult(true, "", false);
	
	
}
