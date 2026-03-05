// copyrighted 2024-2025 - Modfest Studios & Micah A. Parker

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "MapAssetList.generated.h"

/**
 * 
 */
UCLASS()
class FIRSTPERSONMODULE_API UMapAssetList : public UUserWidget
{
	GENERATED_BODY()
public:

	/*if set - will auto-filter all maps to show only ones that match*/
	UPROPERTY(EditAnywhere, Category = "Map List")
		FName FilterCategory;
	UPROPERTY(BlueprintReadOnly, Category = "Map List")
		TArray<class UMapAsset*> Maps;

	/*name your List View widget in your Blueprint this name for it to auto-bind*/
	UPROPERTY(VisibleDefaultsOnly, Category = "Map List")
		FName ListViewWidgetName = "List View";
	UPROPERTY(VisibleDefaultsOnly, Category = "Map List", meta = (BindWidget))
		class UListView* ListView;



	//=================================================================================================================================
	//============================================================FUNCTIONS============================================================
	//=================================================================================================================================

	virtual void NativeConstruct() override;


	UFUNCTION(BlueprintCallable, Category = "Map List")
		virtual void RefreshMapList();
	
};
