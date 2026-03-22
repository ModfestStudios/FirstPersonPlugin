// Fill out your copyright notice in the Description page of Project Settings.


#include "OperatingSystem/Applications/TerminalApplication.h"
#include "OperatingSystem/TerminalCommand.h"
#include "OperatingSystem/Applications/Commands/TerminalCommand_Clear.h"
#include "OperatingSystem/Applications/Commands/TerminalCommand_Echo.h"

ATerminalApplication::ATerminalApplication()
{
	Commands.Add(UTerminalCommand_Clear::StaticClass());
	Commands.Add(UTerminalCommand_Echo::StaticClass());
}

void ATerminalApplication::ExecuteCommand(FString Command)
{
	TArray<FString> CommandArray;
	int32 CommandIndex = -1;
	FTerminalCommandExecutionParameters CommandParameters;

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
			if (Cmd.Contains("-"))
			{
				CommandParameters.Flags.Add(Cmd);
			}
			else
				CommandParameters.Subcommands.Add(FTerminalSubcommand(FName(Cmd)));
		}
	}

	


	if (IsValidCommand(CommandParameters.Command, CommandIndex) && CommandIndex >= 0)
	{
		ExecuteCommandObject(Commands[CommandIndex], CommandParameters);		
	}
	else
	{
		PrintToTerminal(FString::Printf(TEXT("<Error>Invalid Command: %s</>"), *CommandParameters.Command.ToString()));
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
		FTerminalCommandResult Result = Command->GetDefaultObject<UTerminalCommand>()->OnCommandExecuted(this, CommandParameters);

		if (!Result.TerminalMessage.IsEmpty())
			PrintToTerminal(Result.TerminalMessage);
	}
}

void ATerminalApplication::PrintToTerminal(const FString& Message)
{
	TerminalMessages.Add(Message);


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
