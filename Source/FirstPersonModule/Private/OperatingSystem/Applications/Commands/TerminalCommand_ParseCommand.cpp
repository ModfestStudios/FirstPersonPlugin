// Fill out your copyright notice in the Description page of Project Settings.


#include "OperatingSystem/Applications/Commands/TerminalCommand_ParseCommand.h"
#include "OperatingSystem/Applications/TerminalApplication.h"

UTerminalCommand_ParseCommand::UTerminalCommand_ParseCommand()
{
	Command = "parsecommand";
}

FTerminalCommandResult UTerminalCommand_ParseCommand::OnCommandExecuted(ATerminalApplication* Terminal, FTerminalCommandExecutionParameters CommandParameters)
{
	FTerminalCommandResult Result;

	if (CommandParameters.Command.IsNone())
	{
		Result.bSuccess = false;
		Result.TerminalMessage = "No command found";

		return Result;
	}
	Terminal->PrintToTerminal("Command found : " + CommandParameters.Command.ToString(), ETerminalMessageStyle::OK);
	
	/*print off subcommands*/
	Terminal->PrintToTerminal("Printing Subcommands...", ETerminalMessageStyle::Status);	
	for (FTerminalSubcommand& Subcommand : CommandParameters.Subcommands)
	{
		if (Subcommand.Subcommand.IsNone())
			Terminal->PrintToTerminal("Subcommand not found", ETerminalMessageStyle::Error);
		else
			Terminal->PrintToTerminal("Subcommand found : " + Subcommand.Subcommand.ToString(), ETerminalMessageStyle::OK);
	}
	/*print off flags*/
	Terminal->PrintToTerminal("Printing flags...", ETerminalMessageStyle::Status);
	for (FTerminalCommandFlag& Flag : CommandParameters.Flags)
	{
		if (Flag.Flag.IsEmpty())
			Terminal->PrintToTerminal("No flag found", ETerminalMessageStyle::Error);
		else
			Terminal->PrintToTerminal("Flag found : " + Flag.Flag + " | Value : " + Flag.Value, ETerminalMessageStyle::OK);
	}

	return FTerminalCommandResult();
}
