// Fill out your copyright notice in the Description page of Project Settings.


#include "OperatingSystem/Applications/Commands/TerminalCommand_Clear.h"
#include "OperatingSystem/Applications/TerminalApplication.h"

FTerminalCommandResult UTerminalCommand_Clear::OnCommandExecuted(class ATerminalApplication* Terminal, FTerminalCommandExecutionParameters CommandParameters)
{
	/*flag if our Blueprint version has overridden the function*/
	const bool bBlueprintImplementedFunction = GetClass()->IsFunctionImplementedInScript(GET_FUNCTION_NAME_CHECKED(UTerminalCommand, BP_OnCommandExecuted));



	if (bBlueprintImplementedFunction)
		return BP_OnCommandExecuted(Terminal, CommandParameters);
	else if (Terminal)
	{		
		Terminal->ClearTerminal();
		return FTerminalCommandResult(true, "", false);
	}
	else
		return FTerminalCommandResult();
}

UTerminalCommand_Clear::UTerminalCommand_Clear()
{
	Command = "Clear";

}
