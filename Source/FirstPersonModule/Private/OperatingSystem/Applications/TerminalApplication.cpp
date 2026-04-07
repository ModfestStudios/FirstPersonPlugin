// Fill out your copyright notice in the Description page of Project Settings.


#include "OperatingSystem/Applications/TerminalApplication.h"
#include "OperatingSystem/OperatingSystem.h"
#include "OperatingSystem/TerminalCommand.h"
#include "OperatingSystem/Applications/Commands/TerminalCommand_Clear.h"
#include "OperatingSystem/Applications/Commands/TerminalCommand_Echo.h"
#include "OperatingSystem/Applications/Commands/TerminalCommand_WhoAmI.h"
#include "OperatingSystem/Applications/Commands/TerminalCommand_List.h"
#include "OperatingSystem/Applications/Commands/TerminalCommand_qrscanner.h"
#include "OperatingSystem/Applications/Commands/TerminalCommand_touch.h"
#include "OperatingSystem/Applications/Commands/TerminalCommand_gpg.h"
#include "OperatingSystem/Applications/Commands/TerminalCommand_rm.h"
#include "OperatingSystem/Applications/Commands/TerminalCommand_ChangeDirectory.h"
#include "OperatingSystem/Applications/Commands/TerminalCommand_Help.h"
#include "OperatingSystem/Applications/Commands/TerminalCommand_HelloWorld.h"
#include "OperatingSystem/Applications/Commands/TerminalCommand_Exit.h"


ATerminalApplication::ATerminalApplication()
{
	Commands.Add(UTerminalCommand_Clear::StaticClass());
	Commands.Add(UTerminalCommand_Echo::StaticClass());
	Commands.Add(UTerminalCommand_WhoAmI::StaticClass());
	Commands.Add(UTerminalCommand_List::StaticClass());
	Commands.Add(UTerminalCommand_qrscanner::StaticClass());
	Commands.Add(UTerminalCommand_touch::StaticClass());
	Commands.Add(UTerminalCommand_gpg::StaticClass());
	Commands.Add(UTerminalCommand_rm::StaticClass());
	Commands.Add(UTerminalCommand_ChangeDirectory::StaticClass());
	Commands.Add(UTerminalCommand_Help::StaticClass());
	Commands.Add(UTerminalCommand_HelloWorld::StaticClass());
	Commands.Add(UTerminalCommand_Exit::StaticClass());

}

void ATerminalApplication::ExecuteCommand(FString Command)
{
	TArray<FString> CommandArray;
	int32 CommandIndex = -1;
	FTerminalCommandExecutionParameters CommandParameters;
	AOperatingSystem* OS = this->GetOperatingSystem();
	FString ActiveWorkingDirectory = OS->GetActiveWorkingDirectory();
	FString HomeDirectory = OS->GetHomeDirectory();
	PrintToTerminal("<User>white-rabbit</>:<directory>~/" + HomeDirectory + ActiveWorkingDirectory + "</>$ <Default>" + Command + "</>");
	Command.ParseIntoArray(CommandArray,TEXT(" "), true);

	/* combine quoted commands into a single entry */
	for (int32 i = 0; i < CommandArray.Num(); i++)
	{
		FString& Token = CommandArray[i];

		if (!Token.StartsWith(TEXT("\"")))
		{
			continue;
		}

		
		if (Token.Len() >= 2 && Token.EndsWith(TEXT("\"")))
		{
			Token = Token.Mid(1, Token.Len() - 2);
			continue;
		}

		// Start combining
		FString Combined = Token.RightChop(1); // remove opening quote
		int32 EndIndex = i;
		bool bFoundClosingQuote = false;

		for (int32 NextIndex = i + 1; NextIndex < CommandArray.Num(); ++NextIndex)
		{
			
			Combined += TEXT(" ");
			Combined += CommandArray[NextIndex];

			if (CommandArray[NextIndex].EndsWith(TEXT("\"")))
			{
				// Remove closing quote
				Combined.LeftChopInline(1);

				EndIndex = NextIndex;
				bFoundClosingQuote = true;
				break;
			}
		}

		if (bFoundClosingQuote)
		{
			CommandArray[i] = Combined;

			// Remove consumed entries
			const int32 NumToRemove = EndIndex - i;
			CommandArray.RemoveAt(i + 1, NumToRemove, true);
		}
	}


	/*parse out commands, subcommands, and flags*/
	for (int32 i = 0; i < CommandArray.Num(); i++)
	{
		if (i == 0)
		{
			CommandParameters.Command = FName(CommandArray[0]);
			continue;
		}

		if (i > 0)
		{
			FString Cmd = CommandArray[i];
			if (Cmd.Left(1) == "-")
			{
				CommandParameters.Flags.Add(Cmd);
			}
			else
				CommandParameters.Subcommands.Add(FTerminalSubcommand(FName(Cmd)));
		}
	}

	if (BlockedCommand(CommandParameters.Command.ToString())) {
		PrintToTerminal("System: " + CommandParameters.Command.ToString() + ": Permission denied: Command forbidden");
		PrintCommonTerminalResponse(ETerminalCommonMessage::UseHelp);
	}
	else {
		if (IsValidCommand(CommandParameters.Command, CommandIndex) && CommandIndex >= 0)
		{
			ExecuteCommandObject(Commands[CommandIndex], CommandParameters);
		}
		else
		{
			PrintCommonTerminalResponse(ETerminalCommonMessage::CommandNotFound, *CommandParameters.Command.ToString());
			PrintCommonTerminalResponse(ETerminalCommonMessage::UseHelp);
		}
	}
}


bool ATerminalApplication::IsValidCommand(FName Command, int32& CommandIndex)
{
	CommandIndex = -1; //default

	for (int32 i = 0; i < Commands.Num(); i++)
	{
		if (Commands[i] && Commands[i]->GetDefaultObject<UTerminalCommand>()->Command == Command)
		{
			CommandIndex = i;
			return true;
		}			
	}

	return false;
}

void ATerminalApplication::ExecuteCommandObject(TSubclassOf<UTerminalCommand> Command,FTerminalCommandExecutionParameters CommandParameters)
{
	if (Command)
	{
		FTerminalCommandResult Result = Command->GetDefaultObject<UTerminalCommand>()->ExecuteCommand(this, CommandParameters);

		if (!Result.TerminalMessage.IsEmpty())
		{
			if (Result.bSuccess)
			{
				PrintToTerminal(Result.TerminalMessage);
			}
			else
			{
				PrintToTerminal(Result.TerminalMessage, ETerminalMessageStyle::Error);
			}
		}
	}
}

void ATerminalApplication::PrintToTerminal(const FString& Message, ETerminalMessageStyle Style)
{
	switch (Style)
	{
	case ETerminalMessageStyle::None:
		TerminalMessages.Add(Message);
		break;
	case ETerminalMessageStyle::OK:
		TerminalMessages.Add("<OK>[OK]</> " + Message);
		break;
	case ETerminalMessageStyle::Error:
		TerminalMessages.Add("<ERROR>[ERROR]</> " + Message);
		break;
	case ETerminalMessageStyle::Status:
		TerminalMessages.Add("<Status>[STATUS]</> " + Message);
		break;
	}

	RefreshTerminal();
}

void ATerminalApplication::PrintCommonTerminalResponse(ETerminalCommonMessage MessageResponse, FString UserDefinedValue1)
{
	//TODO: Need to cleanup common errors to properly mimic linux common failures;
	switch (MessageResponse)
	{
	case ETerminalCommonMessage::SyntaxLength:
		PrintToTerminal("Syntax too long/short " + UserDefinedValue1, ETerminalMessageStyle::Error);
		break;
	case ETerminalCommonMessage::SyntaxFormat:
		PrintToTerminal("Syntax error invalid format  " + UserDefinedValue1, ETerminalMessageStyle::Error);
		break;
	case ETerminalCommonMessage::CommandNotFound:
		PrintToTerminal(UserDefinedValue1 + ": command not found");
		break;
	case ETerminalCommonMessage::UseHelp:
		PrintToTerminal("For commands available, use: help");
		break;
	}
	RefreshTerminal();
}

void ATerminalApplication::ClearTerminal()
{
	TerminalMessages.Reset();

	RefreshTerminal();
	
}

void ATerminalApplication::RefreshTerminal()
{
	if (OnTerminalUpdated.IsBound())
		OnTerminalUpdated.Broadcast();
}

bool ATerminalApplication::BlockedCommand(const FString& Command)
{
	TArray<FString> BlockedCommands = {
		TEXT("mkdir"),
		TEXT("rmdir"),
		TEXT("chmod"),
		TEXT("systemctl"),
		TEXT("mv"),
		TEXT("nano"),
		TEXT("vi"),
		TEXT("kill"),
		TEXT("killall"),
		TEXT("ifconfig"),
		TEXT("chown"),
		TEXT("bash"),
		TEXT("sh")
	};

	return BlockedCommands.Contains(Command);
}


