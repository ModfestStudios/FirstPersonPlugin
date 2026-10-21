// Test-only observer used by the inventory/interaction automation suites.
#pragma once
#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "Interactives/Interactives.h"
#include "InteractionContextTestReceiver.generated.h"

UCLASS(Transient, NotBlueprintable)
class UInteractionContextTestReceiver : public UObject
{
    GENERATED_BODY()
public:
    int32 InventoryUpdateCount = 0;
    FInteractionContext Started;
    FInteractionContext Ended;
    EInteractionEndReason EndReason = EInteractionEndReason::Completed;
    const UInteractiveAction* LastAction = nullptr;

    UFUNCTION()
    void RecordInventoryUpdated()
    {
        ++InventoryUpdateCount;
    }

    UFUNCTION()
    void RecordStart(AFirstPersonCharacter* User, const UInteractiveAction* Action, const FInteractionContext& Context)
    {
        Started = Context;
        LastAction = Action;
    }
    UFUNCTION()
    void RecordEnd(AFirstPersonCharacter* User, const UInteractiveAction* Action, EInteractionEndReason Reason, const FInteractionContext& Context)
    {
        Ended = Context;
        EndReason = Reason;
        LastAction = Action;
    }
};
