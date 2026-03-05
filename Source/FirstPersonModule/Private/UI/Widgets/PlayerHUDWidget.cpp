// Copyrighted Modfest Studios 2024


#include "UI/Widgets/PlayerHUDWidget.h"

#include "Characters/FirstPersonCharacter.h"

#include "Components/InteractiveManagerComponent.h"



void UPlayerHUDWidget::InitializeHUD(APlayerController* PlayerController, AFirstPersonCharacter* Character)
{
	if (Character && Character->GetInteractionsManager())
	{
		//Character->GetInteractionsManager()->OnInteractiveHoverStateChanged.AddDynamic(this, &UPlayerHUDWidget::OnInteractiveHoverStateChanged);
	}
}

void UPlayerHUDWidget::OnInteractiveHoverStateChanged(AActor* Interactive, AActor* Instigator, bool bHovered)
{
	/*flag our variable - useful for double checking the state outside of our Event Calls*/
	bHoveringInteractable = bHovered;

	/*notify blueprints*/
	if (bHovered)
		BP_OnInteractiveHovered(Interactive);
	else
		BP_OnInteractiveUnhovered(Interactive);

}
