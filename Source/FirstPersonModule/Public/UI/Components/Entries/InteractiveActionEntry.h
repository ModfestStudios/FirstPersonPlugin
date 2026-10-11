// Copyrighted : Modfest Studios 2025-2026

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Interactives/Interactives.h"
#include "Blueprint/IUserObjectListEntry.h"
#include "InteractiveActionEntry.generated.h"

/**
 * 
 */
UCLASS()
class FIRSTPERSONMODULE_API UInteractiveActionEntry : public UUserWidget, public IUserObjectListEntry
{
	GENERATED_BODY()
public:

	
	UPROPERTY(BlueprintReadOnly, Category = "Interactive Action")
		class UInteractiveAction* InteractiveAction;


	//===================================================================================================================================
	//=============================================================FUNCTIONS=============================================================
	//===================================================================================================================================


	/*integration support for List UMG Widgets that automatically calls SetCharacterInfo() for us*/
		virtual void NativeOnListItemObjectSet(UObject* ListItemObject) override;
	
};
