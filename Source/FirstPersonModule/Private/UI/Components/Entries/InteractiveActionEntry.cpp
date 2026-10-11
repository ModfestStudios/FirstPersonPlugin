// Copyrighted : Modfest Studios 2025-2026


#include "UI/Components/Entries/InteractiveActionEntry.h"
#include "Interactives/InteractiveAction.h"

void UInteractiveActionEntry::NativeOnListItemObjectSet(UObject* ListItemObject)
{
	if (UInteractiveAction* InInteractiveAction = Cast<UInteractiveAction>(ListItemObject))
		InteractiveAction = InInteractiveAction;

	/*call Blueprint version*/
	Execute_OnListItemObjectSet(Cast<UObject>(this), ListItemObject);
}
