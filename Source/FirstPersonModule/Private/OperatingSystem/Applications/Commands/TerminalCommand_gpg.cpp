// Fill out your copyright notice in the Description page of Project Settings.


#include "OperatingSystem/Applications/Commands/TerminalCommand_gpg.h"
#include "OperatingSystem/TerminalCommand.h"
#include "OperatingSystem/Applications/TerminalApplication.h"
#include "OperatingSystem/OperatingSystem.h"

FTerminalCommandResult UTerminalCommand_gpg::OnCommandExecuted(ATerminalApplication* Terminal, FTerminalCommandExecutionParameters CommandParameters)
{

	AOperatingSystem* OS = Terminal->GetOperatingSystem();
	FString ActiveWorkingDirectory = OS->GetActiveWorkingDirectory();

	FOperatingSystemFiles FileResult;

	FileResult = OS->FileSystemGetFile("", "", "Tool:gpg");

	if (FileResult.Name == "") {
		Terminal->PrintCommonTerminalResponse(ETerminalCommonMessage::CommandNotFound, "gpg");
		return FTerminalCommandResult();
	}

	Terminal->PrintToTerminal("Checking for private key ...", ETerminalMessageStyle::Status);
	FileResult = OS->FileSystemGetFile("", "", "PlayerPrivateKey");

	if (FileResult.Name == "") {
		Terminal->PrintToTerminal("Could not find private key within /Keys folder", ETerminalMessageStyle::Error);
	}
	else {
		Terminal->PrintToTerminal("Found private key in /Keys/" + FileResult.Name, ETerminalMessageStyle::OK);
	}

	Terminal->PrintToTerminal("Checking for L5 public key ...", ETerminalMessageStyle::Status);
	FileResult = OS->FileSystemGetFile("", "", "Lambda5PublicKey");

	if (FileResult.Name == "") {
		return FTerminalCommandResult(false, "Could not find LAMBDA5 public key within /Keys folder", false);
	}
	else {
		Terminal->PrintToTerminal("Found LAMBDA5 public key in /Keys/" + FileResult.Name, ETerminalMessageStyle::OK);
	}
	return FTerminalCommandResult();
}

UTerminalCommand_gpg::UTerminalCommand_gpg()
{
    Command = "gpg";
}
