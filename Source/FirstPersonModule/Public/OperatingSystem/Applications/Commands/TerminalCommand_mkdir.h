// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "OperatingSystem/TerminalCommand.h"
#include "TerminalCommand_mkdir.generated.h"

/** TODO: Not going to have directory creation at this time; not necessary for gameplay and adds complexity of managing removals of directories w/ sub content
 * 
 */
UCLASS()
class FIRSTPERSONMODULE_API UTerminalCommand_mkdir : public UTerminalCommand
{
	GENERATED_BODY()
	
};
