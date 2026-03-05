// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/Widgets/MapSelectionWidget.h"
#include "Subsystems/ModSubsystem.h"
#include "Maps/MapAsset.h"

/*engine*/
#include "Engine/GameInstance.h"

void UMapSelectionWidget::InitMapListByCategory(const FName Category)
{	
	if (UModSubsystem* ModSubsystem = GetGameInstance()->GetSubsystem<UModSubsystem>())
	{
		Maps = ModSubsystem->GetMapsByCategory(Category);
	}		
}
