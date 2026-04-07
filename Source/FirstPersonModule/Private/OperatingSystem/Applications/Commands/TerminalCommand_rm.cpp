// Fill out your copyright notice in the Description page of Project Settings.


#include "OperatingSystem/Applications/Commands/TerminalCommand_rm.h"
#include "OperatingSystem/TerminalCommand.h"
#include "OperatingSystem/Applications/TerminalApplication.h"
#include "OperatingSystem/OperatingSystem.h"

FTerminalCommandResult UTerminalCommand_rm::OnCommandExecuted(ATerminalApplication* Terminal, FTerminalCommandExecutionParameters CommandParameters)
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
		
		if (FileName.StartsWith(".")) {
			FileName = FileName.RightChop(1);
		}

		if (FileName.Contains("/")) {
			return FTerminalCommandResult(false, "Invalid filename: Files cannot contain /", false);
		}
		if (!fileExists) {
			return FTerminalCommandResult(false, "File " + FileName + " does not exist", true);
		}
		else
		{
			OS->FileSystemDeleteFile(FileName, ActiveWorkingDirectory, FOperatingSystemFileType::File);
			Terminal->PrintToTerminal("File removed successfully", ETerminalMessageStyle::OK);
			return FTerminalCommandResult();
		}
	}
}

UTerminalCommand_rm::UTerminalCommand_rm()
{
	Command = "rm";
}
