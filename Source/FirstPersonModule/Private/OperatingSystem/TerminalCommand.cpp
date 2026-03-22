// Fill out your copyright notice in the Description page of Project Settings.


#include "OperatingSystem/TerminalCommand.h"
#include "OperatingSystem/Applications/TerminalApplication.h"
#include "OperatingSystem/OperatingSystem.h"

FTerminalCommandResult UTerminalCommand::OnCommandExecuted(ATerminalApplication* Terminal, FTerminalCommandExecutionParameters CommandParameters)
{
	/*flag if our Blueprint version has overridden the function*/
	const bool bBlueprintImplementedFunction = GetClass()->IsFunctionImplementedInScript(GET_FUNCTION_NAME_CHECKED(UTerminalCommand, BP_OnCommandExecuted));
	
	

	if (bBlueprintImplementedFunction)
		return BP_OnCommandExecuted(Terminal, CommandParameters);
	else
		return FTerminalCommandResult();
}
