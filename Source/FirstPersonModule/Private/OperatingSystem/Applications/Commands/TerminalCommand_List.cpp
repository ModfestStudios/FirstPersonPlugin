// Fill out your copyright notice in the Description page of Project Settings.


#include "OperatingSystem/Applications/Commands/TerminalCommand_List.h"
#include "OperatingSystem/Applications/TerminalApplication.h"
#include "OperatingSystem/OperatingSystem.h"

FTerminalCommandResult UTerminalCommand_List::OnCommandExecuted(ATerminalApplication* Terminal, FTerminalCommandExecutionParameters CommandParameters)
{
	TArray<FOperatingSystemFiles> FileResults;
	FString CurrentFlag;
	bool ShowHidden = false;
	bool SortBySize = false;
	bool SortByDate = false;
	bool ShowSubs = false; //TODO: Need to setup -R flag for listing subdirectory + files
	bool IsInDirectoryFilter = true;

	if (Terminal)
	{
		//TODO: Old directory process ; remove?
			//TArray<FOperatingSystemDirectory> Directories = Terminal->GetOperatingSystem()->GetAllRootDirectories();
		TArray<FOperatingSystemFiles> Files;
		AOperatingSystem* OS = Terminal->GetOperatingSystem();
		FString ActiveWorkingDirectory = OS->GetActiveWorkingDirectory();
		bool IsRootFile = false;

		for (FTerminalCommandFlag& Flag : CommandParameters.Flags)
		{
			if (Flag == "help")
			{
				Terminal->PrintToTerminal(getHelpText());
				return FTerminalCommandResult();
			}

			if (Flag == "art")
			{
				ShowHidden = true;
			}

			if (Flag == "ls")
			{
				SortBySize = true;
			}

			if (Flag == "t")
			{
				SortByDate = true;
			}
		}

		/*for (int32 i = 0; i < CommandParameters.Flags.Num(); i++) {
			CurrentFlag = CommandParameters.Flags[i];
			if (CurrentFlag == "--help") {
				Terminal->PrintToTerminal(getHelpText());
				return FTerminalCommandResult();
			}
			if (CurrentFlag == "-art") {
				ShowHidden = true;
			}
			if (CurrentFlag.Equals("-lS", ESearchCase::CaseSensitive)) {
				SortBySize = true;
			}
			if (CurrentFlag == "-t") {
				SortByDate = true;
			}

		}*/

		if (SortBySize && SortByDate) {
			Terminal->PrintCommonTerminalResponse(ETerminalCommonMessage::SyntaxLength, " -t and -lS not compatible");
			return FTerminalCommandResult();
		}

		if (SortBySize) {
			Files = Terminal->GetOperatingSystem()->FileSystemListFiles(FOperatingSystemFileSortBy::Size);
			goto ContinueFileList;
		}
		if (SortByDate) {
			Files = Terminal->GetOperatingSystem()->FileSystemListFiles(FOperatingSystemFileSortBy::Date);
			goto ContinueFileList;
		}
		else {
			Files = Terminal->GetOperatingSystem()->FileSystemListFiles();
		}
		ContinueFileList:

		//TODO: Old directory process ; remove?
		/*
		if (Directories.Num() == 0)
			return FTerminalCommandResult(false, "NO RESULTS FOUND", false);

		for (int32 i = 0; i < Directories.Num(); i++)
		{
			Terminal->PrintToTerminal("<Directory>" + Directories[i].Label + "</>");
		}
		*/

		for (int32 i = 0; i < Files.Num(); i++) {
			FString HiddenPrefix;
			//DEBUG: Used to check file arrays as needed
			//GEngine->AddOnScreenDebugMessage(-1, 10.f, FColor::Green, TEXT("File: " + Files[i].Name));
			if (Files[i].Path == ActiveWorkingDirectory && !ShowSubs) {
				IsInDirectoryFilter = true;
				FileResults.Add(Files[i]);
			}
			else { IsInDirectoryFilter = false; }
		}

			//TODO: Need to discuss; intentionally having this count hidden files to be a clue to players there is more than seen if not using show hidden files switch
			Terminal->PrintToTerminal("Total: " + FString::FromInt(FileResults.Num()));
			FString Header = FString::Printf(TEXT("%-13s | %-8s | %-13s | %s"), TEXT("Permissions"), TEXT("Size"), TEXT("Date"), TEXT("File"));
			Terminal->PrintToTerminal(Header);
			for (int32 i = 0; i < FileResults.Num(); i++) {
				FString HiddenPrefix;
				if (!ShowHidden && FileResults[i].bIsHidden) { continue; }
				if (ShowHidden && FileResults[i].bIsHidden) { HiddenPrefix = "."; }
				FString Permissions = FileResults[i].Permissions;
				FString Size = FString::FromInt(FileResults[i].Size);
				FString Date = FileResults[i].Date.ToString(TEXT("%Y-%m-%d"));
				FString Name;
				if (FileResults[i].Type == FOperatingSystemFileType::Directory) {
					Name = "<Directory>" + HiddenPrefix + FileResults[i].Name + "</>";
				}
				else {
					Name = HiddenPrefix + FileResults[i].Name;
				}

				FString Line = FString::Printf(
					TEXT("%-13s | %-8s | %-13s | %s"),
					*Permissions,
					*Size,
					*Date,
					*Name
				);
				Terminal->PrintToTerminal(Line);
			}
		}
		return FTerminalCommandResult();
}

FString UTerminalCommand_List::getHelpText() const {
	return TEXT(R"HELP(
NAME
    ls - list files and directories in current directory

SYNOPSIS
    ls [OPTIONS]

DESCRIPTION
    modified list command to provide a list of available
	files and directories within the current directory.
	sorts entries alphabetically if no sort option is used.
	note: options are case sensitive
	note: directories list in blue text, files in system grey.

OPTIONS
	--help
		display this help file

    -lS
        sort by size (ascending)

    -t
        sort by date (ascending)

    -art
		show hidden files and directories
		note: hidden denoted by a prefix of '.' to name


EXAMPLE

    ls -lS

)HELP");
}

UTerminalCommand_List::UTerminalCommand_List()
{
	Command = "ls";
}
