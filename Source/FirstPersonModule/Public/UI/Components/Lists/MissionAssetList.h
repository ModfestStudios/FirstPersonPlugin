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
		bool bAutoPopulateMissions = true;
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Missions")
		bool bExcludeTrainingMissions = true;
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Missions")
		EMissionListType MissionsFilter;
	
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Missions", meta = (EditCondition="!bAutoPopulateMissions"))
		TArray<class UMissionAsset*> Missions;

	//=========================================================================================================================================
	//================================================================FUNCTIONS================================================================
	//=========================================================================================================================================

	virtual void NativeConstruct() override;

	UFUNCTION(BlueprintCallable, Category = "Missions")
		virtual void RefreshMissionList();
	UFUNCTION(BlueprintImplementableEvent, Category = "Missions", meta = (DisplayName = "On Mission List Refreshed"))
		void BP_OnRefreshMissionList();
	
};
