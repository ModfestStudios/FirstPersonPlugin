// copyrighted 2024-2025 - Modfest Studios & Micah A. Parker

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Blueprint/IUserObjectListEntry.h"
#include "MissionAssetEntry.generated.h"

/**
 * 
 */
UCLASS()
class FIRSTPERSONMODULE_API UMissionAssetEntry : public UUserWidget, public IUserObjectListEntry
{
	GENERATED_BODY()
public:

	UPROPERTY(BlueprintReadOnly, Category = "Mission")
		class UMissionAsset* Mission;

	//===========================================================================================================================
	//=========================================================FUNCTIONS=========================================================
	//===========================================================================================================================

	/*object is passed in - should be converted to UMapInfo and setse MapInfo*/
	virtual void NativeOnListItemObjectSet(UObject* ListItemObject) override;
	/*selection state*/
	virtual void NativeOnItemSelectionChanged(bool bIsSelected) override;



	/*sets this as the selected mission in the UI*/
	UFUNCTION(BlueprintCallable, Category = "Mission")
		virtual void SelectMission();
	UFUNCTION(BlueprintCallable, Category = "Mission")
		virtual void UnselectMission();
	
};
