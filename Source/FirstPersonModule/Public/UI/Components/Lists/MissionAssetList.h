// copyrighted 2024-2025 - Modfest Studios & Micah A. Parker

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Subsystems/MissionSubsystem.h"
#include "MissionAssetList.generated.h"

/**
 * 
 */
UCLASS()
class FIRSTPERSONMODULE_API UMissionAssetList : public UUserWidget
{
	GENERATED_BODY()
public:

	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Missions")
		EMissionListType MissionsFilter;
	UPROPERTY(BlueprintReadOnly, Category = "Missions")
		TArray<class UMissionAsset*> Missions;

	//=========================================================================================================================================
	//================================================================FUNCTIONS================================================================
	//=========================================================================================================================================


	UFUNCTION(BlueprintCallable, Category = "Missions")
		virtual void RefreshMissionList();
	UFUNCTION(BlueprintImplementableEvent, Category = "Missions", meta = (DisplayName = "On Mission List Refreshed"))
		void BP_OnRefreshMissionList();
	
};
