// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/Widgets/UserSettingsWidget.h"
#include "GameFramework/GameUserSettings.h"
#include "RHI.h"

FIntPoint UUserSettingsWidget::GetScreenResolution()
{
	return GetUserSettings()->GetScreenResolution();	
}

FIntPoint UUserSettingsWidget::GetDesktopResolution()
{
	return GetUserSettings()->GetDesktopResolution();
}

TArray<FIntPoint> UUserSettingsWidget::GetSupportedScreenResolutions()
{
	/*init*/
	TArray<FIntPoint> SupportedResolutions;
	FScreenResolutionArray Resolutions;
	
	if (RHIGetAvailableResolutions(Resolutions, true))
	{
		for (auto Result : Resolutions)
		{
			SupportedResolutions.Add(FIntPoint(Result.Width, Result.Height));
		}
	}

	return SupportedResolutions;
}

void UUserSettingsWidget::SetScreenResolution(FIntPoint NewResolution)
{
	GetUserSettings()->SetScreenResolution(NewResolution);
	GetUserSettings()->ApplyResolutionSettings(false);
}

EWindowMode::Type UUserSettingsWidget::GetWindowMode()
{
	return GetUserSettings()->GetFullscreenMode();
}

void UUserSettingsWidget::SetWindowMode(EWindowMode::Type Mode)
{
	GetUserSettings()->SetFullscreenMode(Mode);
	//GetUserSettings()->ApplySettings(false);
	GetUserSettings()->ApplyResolutionSettings(false);
}
