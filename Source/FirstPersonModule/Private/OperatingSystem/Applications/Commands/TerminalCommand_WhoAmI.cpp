// Fill out your copyright notice in the Description page of Project Settings.


#include "OperatingSystem/Applications/Commands/TerminalCommand_WhoAmI.h"
#include "OperatingSystem/Applications/TerminalApplication.h"
#include "OperatingSystem/OperatingSystem.h"

FTerminalCommandResult UTerminalCommand_WhoAmI::OnCommandExecuted(ATerminalApplication* Terminal, FTerminalCommandExecutionParameters CommandParameters)
{
	
	if (Terminal)
	{
		if (AOperatingSystem* OS = Terminal->GetOperatingSystem())
		{
			FString User = OS->GetCurrentUser();

			if (User.IsEmpty())
				return FTerminalCommandResult(false, "no user found", false);
			else
				return FTerminalCommandResult(true, User, false);
		}
	}
	
	return FTerminalCommandResult(false,"Failed to find user",false);
}

UTerminalCommand_WhoAmI::UTerminalCommand_WhoAmI()
{
	Command = "whoami";
}
