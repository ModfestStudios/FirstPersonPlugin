// Copyrighted Modfest Studios 2024

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "PlayerHUDWidget.generated.h"

/**
 *
 */
UCLASS()
class FIRSTPERSONMODULE_API UPlayerHUDWidget : public UUserWidget
{
	GENERATED_BODY(abstract)
public:

	UPROPERTY(BlueprintReadWrite, Category = "HUD")
		bool bMinimalHud = false;
	/*indicates whether or not this player is hovering over an interactive item or not - does not get set for disabled interactives*/
	UPROPERTY(BlueprintReadOnly, Category = "Interactions")
		bool bHoveringInteractable = false;


	//=============================================================================================================================
	//==========================================================FUNCTIONS==========================================================
	//=============================================================================================================================
public:
	virtual void InitializeHUD(APlayerController* PlayerController, class AFirstPersonCharacter* Character);

public:
	UFUNCTION()
		virtual void OnInteractiveHoverStateChanged(AActor* Interactive, AActor* Instigator, bool bHovered);
protected:
	UFUNCTION(BlueprintImplementableEvent, meta = (DisplayName = "On Interactive Hovered"))
		void BP_OnInteractiveHovered(AActor* InteractiveActor);
	UFUNCTION(BlueprintImplementableEvent, meta = (DisplayName = "On Interactive Unhovered"))
		void BP_OnInteractiveUnhovered(AActor* InteractiveActor);



};
