// Fill out your copyright notice in the Description page of Project Settings.


#include "OperatingSystem/Applications/Commands/TerminalCommand_Clear.h"
#include "OperatingSystem/Applications/TerminalApplication.h"

FTerminalCommandResult UTerminalCommand_Clear::OnCommandExecuted(class ATerminalApplication* Terminal, FTerminalCommandExecutionParameters CommandParameters)
{
	Terminal->ClearTerminal();
	return FTerminalCommandResult(true, "", false);	
}

UTerminalCommand_Clear::UTerminalCommand_Clear()
{
	Command = "Clear";
}
