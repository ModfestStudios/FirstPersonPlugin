// Copyrighted : Modfest Studios 2025-2026

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Interactives/Interactives.h"
#include "InteractiveActionList.generated.h"


/**
 * 
 */
UCLASS()
class FIRSTPERSONMODULE_API UInteractiveActionList : public UUserWidget
{
	GENERATED_BODY()
public:





	UPROPERTY(BlueprintReadOnly, Category = "Interactive Actions")
		TArray<class UInteractiveAction*> Actions;



	//=======================================================================================================================================
	//===============================================================FUNCTIONS===============================================================
	//=======================================================================================================================================

	

	virtual void NativeConstruct() override;

	UFUNCTION()
		virtual void NativeOnListRefreshed();
	UFUNCTION(BlueprintImplementableEvent, Category = "Interaction Actions", meta = (DisplayName = "On List Refreshed"))
		void BP_OnListRefreshed();


	UFUNCTION(BlueprintCallable, Category = "Interactive Actions")
		virtual void SetInteractiveActionList(TArray<UInteractiveAction*> InteractionActionList);




	
};
