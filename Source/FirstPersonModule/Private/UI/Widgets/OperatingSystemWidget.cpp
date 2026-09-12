// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/Widgets/OperatingSystemWidget.h"

UOperatingSystemWidget::UOperatingSystemWidget(const FObjectInitializer& ObjectInitializer)
	:Super(ObjectInitializer)
{
	SetIsFocusable(true);
}

void UOperatingSystemWidget::InitializeWidget(AOperatingSystem* OS)
{
	OperatingSystem = OS;
}

ATerminalSystem* UOperatingSystemWidget::GetTerminalSystem()
{
	if (OperatingSystem)
		return OperatingSystem->GetTerminalSystem();
	else
		return nullptr;
}

EOperatingSystemMainState UOperatingSystemWidget::GetOperatingSystemState()
{
	if (OperatingSystem)
		return OperatingSystem->GetSystemState();
	return EOperatingSystemMainState();
}
