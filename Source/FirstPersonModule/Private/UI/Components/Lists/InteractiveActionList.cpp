// Copyrighted : Modfest Studios 2025-2026


#include "UI/Components/Lists/InteractiveActionList.h"
#include "Interactives/InteractiveAction.h"

void UInteractiveActionList::NativeConstruct()
{
    Super::NativeConstruct();
}

void UInteractiveActionList::NativeOnListRefreshed()
{
    BP_OnListRefreshed();
}

void UInteractiveActionList::SetInteractiveActionList(TArray<UInteractiveAction*> InteractionActionList)
{
    if (Actions != InteractionActionList) 
    {
        Actions = InteractionActionList;
        NativeOnListRefreshed();
    }    
}
