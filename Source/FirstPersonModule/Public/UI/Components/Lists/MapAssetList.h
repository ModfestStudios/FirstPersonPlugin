// copyrighted 2024-2025 - Modfest Studios & Micah A. Parker

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "MapAssetList.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnMapListUpdated);

/**
 * 
 */
UCLASS()
class FIRSTPERSONMODULE_API UMapAssetList : public UUserWidget
{
	GENERATED_BODY()
public:

	/*when true, will automatically pull all maps of desired type into the Maps Array*/
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Map List")
		bool bAutoPopulateMaps = true;
	/*will only return maps of the set type*/
	UPROPERTY(BlueprintReadOnly,EditAnywhere, Category = "Map List")
		TSubclassOf<class UMapAsset> ClassFilter;
	/*if set - will auto-filter all maps to show only ones that match*/
	UPROPERTY(EditAnywhere, Category = "Map List")
		FName FilterCategory;
	UPROPERTY(BlueprintReadOnly, Category = "Map List")
		TArray<class UMapAsset*> Maps;

	/*name your List View widget in your Blueprint this name for it to auto-bind*/
	UPROPERTY(VisibleDefaultsOnly, Category = "Map List")
		FName ListViewWidgetName = "List View";
	//UPROPERTY(VisibleDefaultsOnly, Category = "Map List", meta = (BindWidget))
	//	class UListView* ListView;


	/*events*/
	UPROPERTY(BlueprintAssignable)
		FOnMapListUpdated OnMapListUpdated;

	//=================================================================================================================================
	//============================================================FUNCTIONS============================================================
	//=================================================================================================================================

	UMapAssetList(const FObjectInitializer& ObjectInitializer);

	virtual void NativeConstruct() override;

	UFUNCTION(BlueprintImplementableEvent, Category = "Map List", meta = (DisplayName = "On Map List Updated"))
		void BP_OnMapListUpdated();


	UFUNCTION(BlueprintCallable, Category = "Map List")
		virtual void RefreshMapList();
	
};
