// Fill out your copyright notice in the Description page of Project Settings.


#include "OperatingSystem/Applications/Commands/TerminalCommand_List.h"
#include "OperatingSystem/Applications/TerminalApplication.h"
#include "OperatingSystem/OperatingSystem.h"

FTerminalCommandResult UTerminalCommand_List::OnCommandExecuted(ATerminalApplication* Terminal, FTerminalCommandExecutionParameters CommandParameters)
{
	if (Terminal)
	{
		TArray<FOperatingSystemDirectory> Directories = Terminal->GetOperatingSystem()->GetAllRootDirectories();

		if (Directories.Num() == 0)
			return FTerminalCommandResult(false, "NO RESULTS FOUND", false);

		for (int32 i = 0; i < Directories.Num(); i++)
		{
			Terminal->PrintToTerminal(Directories[i].Label);
		}
	}
	
	return FTerminalCommandResult();
}

UTerminalCommand_List::UTerminalCommand_List()
{
	Command = "ls";
}
