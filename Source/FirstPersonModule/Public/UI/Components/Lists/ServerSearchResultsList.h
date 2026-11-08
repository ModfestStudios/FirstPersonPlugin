// Copyrighted : Modfest Studios 2025-2026

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "ServerSearchResultsList.generated.h"

class UServerBrowserSearchResult;

/**
 * 
 */
UCLASS()
class FIRSTPERSONMODULE_API UServerSearchResultsList : public UUserWidget
{
	GENERATED_BODY()
public:


	UPROPERTY(BlueprintReadOnly, Category = "Search Results")
		TArray<UServerBrowserSearchResult*> SearchResults;

	/*when enabled automatically binds itself to the Server Subsystem search system*/
	UPROPERTY(EditAnywhere, Category = "Server Search Settings")
		bool bAutoBindToServerResults = true;



	//=================================================================================================================================
	//============================================================FUNCTIONS============================================================
	//=================================================================================================================================

	UServerSearchResultsList(const FObjectInitializer& ObjectInitializer);

	virtual void NativeConstruct() override;

	UFUNCTION()
		virtual void NativeOnSearchResultsUpdated();

	UFUNCTION(BlueprintImplementableEvent, Category = "Map List", meta = (DisplayName = "On Search Results Updated"))
		void BP_OnSearchResultsUpdated();
	
};
