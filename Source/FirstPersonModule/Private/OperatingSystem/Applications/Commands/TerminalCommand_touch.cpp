// Fill out your copyright notice in the Description page of Project Settings.


#include "OperatingSystem/Applications/Commands/TerminalCommand_touch.h"
#include "OperatingSystem/TerminalCommand.h"
#include "OperatingSystem/Applications/TerminalApplication.h"
#include "OperatingSystem/OperatingSystem.h"

FTerminalCommandResult UTerminalCommand_touch::OnCommandExecuted(ATerminalApplication* Terminal, FTerminalCommandExecutionParameters CommandParameters)
{
	if (CommandParameters.Subcommands.Num() != 1) {
		Terminal->PrintCommonTerminalResponse(ETerminalCommonMessage::SyntaxLength);
		return FTerminalCommandResult();
	}
	else {
		FString FileName = CommandParameters.Subcommands[0].Subcommand.ToString();
		AOperatingSystem* OS = Terminal->GetOperatingSystem();
		FString ActiveWorkingDirectory = OS->GetActiveWorkingDirectory();
		bool fileExists = OS->FileSystemCheckIfFileExists(FileName, ActiveWorkingDirectory, FOperatingSystemFileType::File);
		if (FileName.Contains("/")) {
			return FTerminalCommandResult(false, "Invalid filename: Files cannot contain /", false);
		}
		if (fileExists) {
			return FTerminalCommandResult(false,"File already exists",true);
		} else
		{
			if (FileName.StartsWith(".")) {
				OS->FileSystemAddFile(FileName.RightChop(1), FDateTime::Now(), ActiveWorkingDirectory, 0, "rx-xx-rf", FOperatingSystemFileType::File, true);
			}
			else {	
				OS->FileSystemAddFile(FileName, FDateTime::Now(), ActiveWorkingDirectory, 0, "rx-xx-rf", FOperatingSystemFileType::File, false);
			}
			return FTerminalCommandResult();
		}
	}
}

UTerminalCommand_touch::UTerminalCommand_touch()
{
	Command = "touch";
}
