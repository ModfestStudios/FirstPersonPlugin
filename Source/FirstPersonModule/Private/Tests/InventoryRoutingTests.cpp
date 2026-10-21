// Copyright (c) Modfest Studios
#if WITH_DEV_AUTOMATION_TESTS
#include "Misc/AutomationTest.h"
#include "Engine/World.h"
#include "Engine/Engine.h"
#include "Engine/LocalPlayer.h"
#include "GameFramework/PlayerController.h"
#include "Engine/EngineBaseTypes.h"
#include "Components/SceneComponent.h"
#include "Components/InventoryManagerComponent.h"
#include "Components/InventoryItemComponent.h"
#include "Components/InventoryAttachmentComponent.h"
#include "Inventory/Items/Battery.h"
#include "Inventory/InventoryItemSlot.h"
#include "Characters/FirstPersonCharacter.h"
#include "Components/PlayerInventoryManagerComponent.h"
#include "Components/InteractiveManagerComponent.h"
#include "Components/InteractiveComponent.h"
#include "Interactives/Actions/PickupAction.h"
#include "UObject/UnrealType.h"
#include "InteractionContextTestReceiver.h"
#include "UI/Components/Entries/InventoryAttachmentCompEntry.h"
#include "Subsystems/InventorySubsystem.h"
#include "UObject/StrongObjectPtr.h"

namespace InventoryRoutingTests
{
struct FWorldFixture
{
    UWorld* World;
    FWorldFixture()
    {
        const UWorld::InitializationValues Values = UWorld::InitializationValues()
            .AllowAudioPlayback(false).CreatePhysicsScene(true).CreateNavigation(false)
            .CreateAISystem(false).ShouldSimulatePhysics(false).SetTransactional(false);
        World = UWorld::CreateWorld(EWorldType::Game, false, NAME_None, nullptr, true, ERHIFeatureLevel::Num, &Values);
        GEngine->CreateNewWorldContext(EWorldType::Game).SetCurrentWorld(World);
        World->InitializeActorsForPlay(FURL());
    }
    ~FWorldFixture()
    {
        World->DestroyWorld(false);
        GEngine->DestroyWorldContext(World);
    }

    AActor* SpawnOwner()
    {
        AActor* Actor = World->SpawnActor<AActor>();
        USceneComponent* Root = NewObject<USceneComponent>(Actor);
        Actor->SetRootComponent(Root);
        Root->RegisterComponent();
        return Actor;
    }
    ABattery* SpawnItem() { return World->SpawnActor<ABattery>(); }
    UInventoryManagerComponent* Manager(AActor* Owner, uint8 Capacity,
        TArray<FInventoryEquipmentSlotConfig> Slots = {})
    {
        UInventoryManagerComponent* Result = NewObject<UInventoryManagerComponent>(Owner);
        Result->SlotCapacity = Capacity;
        Result->EquipmentSlotConfigs = Slots;
        Result->RegisterComponent();
        Result->InitializeEquipmentSlots();
        return Result;
    }
};
FInventoryEquipmentSlotConfig Slot(FName ID, EInventoryEquipmentSlotKind Kind)
{
    FInventoryEquipmentSlotConfig Result;
    Result.SlotID = ID;
    Result.SlotKind = Kind;
    if (Kind == EInventoryEquipmentSlotKind::Equipment)
        Result.AcceptedItemClass = ABattery::StaticClass();
    return Result;
}
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FInventoryRoutingTest,
    "FirstPersonModule.Inventory.RoutingAndAtomicTransfers",
    EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FInventoryRoutingTest::RunTest(const FString& Parameters)
{
    using namespace InventoryRoutingTests;
    FWorldFixture Fixture;
    AActor* Player = Fixture.SpawnOwner();
    UInventoryManagerComponent* Root = Fixture.Manager(Player, 0, {
        Slot("Backpack", EInventoryEquipmentSlotKind::Equipment),
        Slot("MainHand", EInventoryEquipmentSlotKind::MainHand)});
    ABattery* Backpack = Fixture.SpawnItem();
    UInventoryManagerComponent* Bag = Fixture.Manager(Backpack, 2, {
        Slot("Grenade", EInventoryEquipmentSlotKind::Equipment)});
    TestTrue(TEXT("long press equips a compatible container"), Root->PickupItem(Backpack, FInteractionContext(true)));
    TestEqual(TEXT("container occupies backpack slot"), Root->GetEquipmentSlot("Backpack")->GetAttachedItem(),
              static_cast<AInventoryItem*>(Backpack));
    TestEqual(TEXT("player has no own storage"), Root->GetInventorySlots().Num(), 0);

    ABattery* Grenade = Fixture.SpawnItem();
    TestTrue(TEXT("nested specialized equipment precedes hands"), Root->PickupItem(Grenade, FInteractionContext(true)));
    TestEqual(TEXT("grenade occupies container's slot"), Bag->GetEquipmentSlot("Grenade")->GetAttachedItem(),
              static_cast<AInventoryItem*>(Grenade));
    TestNull(TEXT("main hand is still empty"), Root->GetEquippedItem());

    ABattery* Held = Fixture.SpawnItem();
    TestTrue(TEXT("long press falls back to main hand"), Root->PickupItem(Held, FInteractionContext(true)));
    TestEqual(TEXT("held item is not stored"), Root->GetInventorySlots().Num(), 0);
    ABattery* Rejected = Fixture.SpawnItem();
    const FTransform Before = Rejected->GetActorTransform();
    TestFalse(TEXT("occupied equipment and hands reject long press"), Root->PickupItem(Rejected, FInteractionContext(true)));
    TestEqual(TEXT("failed pickup preserves presence"), Rejected->GetItemComponent()->GetItemPresence(), EItemPresence::World);
    TestTrue(TEXT("failed pickup preserves transform"), Rejected->GetActorTransform().Equals(Before));
    TestNull(TEXT("failed pickup preserves owner"), Rejected->GetOwner());
    TestFalse(TEXT("failed pickup preserves visibility"), Rejected->IsHidden());

    Root->DropItem(Held);
    TestNull(TEXT("drop clears hand slot"), Root->GetEquippedItem());
    TestEqual(TEXT("drop restores world presence"), Held->GetItemComponent()->GetItemPresence(), EItemPresence::World);

    ABattery* Stored = Fixture.SpawnItem();
    TestTrue(TEXT("short press stores in equipped container"), Root->PickupItem(Stored, FInteractionContext(false)));
    TestEqual(TEXT("stored item's actual manager is the bag"), Stored->GetItemComponent()->GetInventoryManager(), Bag);
    TestTrue(TEXT("base stored item is hidden"), Stored->IsHidden());
    TestNull(TEXT("short press does not use a hand"), Root->GetEquippedItem());
    TestTrue(TEXT("equipping releases storage capacity"), Root->TryEquipItem(Stored));
    TestEqual(TEXT("storage entry removed when held"), Bag->GetInventorySlots().Num(), 0);
    TestEqual(TEXT("previous storage remembered"), Stored->GetItemComponent()->GetPreviousStorage(), Bag);
    TestTrue(TEXT("put away returns to previous storage"), Root->TryPutAwayItem(Stored));
    TestEqual(TEXT("restored to original manager"), Stored->GetItemComponent()->GetInventoryManager(), Bag);

    TestTrue(TEXT("equip stored item again"), Root->TryEquipItem(Stored));
    ABattery* FillOne = Fixture.SpawnItem();
    ABattery* FillTwo = Fixture.SpawnItem();
    TestTrue(TEXT("first item fills container"), Root->PickupItem(FillOne, FInteractionContext(false)));
    TestTrue(TEXT("second item fills container"), Root->PickupItem(FillTwo, FInteractionContext(false)));
    TestFalse(TEXT("no room rejects put away without releasing hand"), Root->TryPutAwayItem(Stored));
    TestEqual(TEXT("failed put away keeps item in hand"), Root->GetEquippedItem(), static_cast<AInventoryItem*>(Stored));
    TestFalse(TEXT("full inventory rejects short press"), Root->PickupItem(Rejected, FInteractionContext(false)));
    TestEqual(TEXT("rejected short press leaves item in world"), Rejected->GetItemComponent()->GetItemPresence(), EItemPresence::World);

    Root->DropItem(Backpack);
    TestEqual(TEXT("container contents stay inside dropped container"), Bag->GetInventorySlots().Num(), 2);
    TestEqual(TEXT("attached grenade travels with container"), Bag->GetEquipmentSlot("Grenade")->GetAttachedItem(),
              static_cast<AInventoryItem*>(Grenade));
    TestNull(TEXT("removed container no longer contributes storage"), Root->FindStorageForItem(Rejected));
    TestFalse(TEXT("previous storage on removed bag cannot be used"), Root->TryPutAwayItem(Stored));
    TestEqual(TEXT("failed return keeps held item"), Root->GetEquippedItem(), static_cast<AInventoryItem*>(Stored));

    TestTrue(TEXT("cycle prevention rejects self"), Bag->WouldCreateContainmentCycle(Backpack));
    TestFalse(TEXT("container cannot store itself"), Bag->AddItem(Backpack));
    // Reattach bag, then try inserting an ancestor into its descendant.
    TestTrue(TEXT("container can be re-equipped"), Root->PickupItem(Backpack, FInteractionContext(true)));
    UInventoryManagerComponent* GrenadeContainer = Fixture.Manager(Grenade, 20);
    TestFalse(TEXT("automatic searches do not use third-level storage"), Root->PickupItem(Rejected, FInteractionContext(false)));
    TestTrue(TEXT("cycle prevention rejects ancestor"), GrenadeContainer->WouldCreateContainmentCycle(Backpack));
    TestFalse(TEXT("ancestor insertion is rejected"), GrenadeContainer->AddItem(Backpack));

    UInventoryAttachmentComponent* RuntimeSlot = Root->GetEquipmentSlot("Backpack");
    Root->DestroyComponent();
    TestFalse(TEXT("runtime slots destroyed with manager"), IsValid(RuntimeSlot));
    TestTrue(TEXT("slot cleanup does not destroy container"), IsValid(Backpack));
    TestEqual(TEXT("slot cleanup preserves container contents"), Bag->GetInventorySlots().Num(), 2);
    return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FInventoryNearbyManagersTest,
    "FirstPersonModule.Inventory.NearbyManagerOwnership",
    EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FInventoryNearbyManagersTest::RunTest(const FString& Parameters)
{
    using namespace InventoryRoutingTests;
    FWorldFixture Fixture;
    AActor* Player = Fixture.SpawnOwner();
    UInventoryManagerComponent* Root = Fixture.Manager(Player, 0, {Slot("Backpack", EInventoryEquipmentSlotKind::Equipment)});
    ABattery* Backpack = Fixture.SpawnItem();
    UInventoryManagerComponent* Bag = Fixture.Manager(Backpack, 0, {Slot("Nested", EInventoryEquipmentSlotKind::Equipment)});
    ABattery* NestedItem = Fixture.SpawnItem();
    UInventoryManagerComponent* Nested = Fixture.Manager(NestedItem, 1);
    UInventoryManagerComponent* WorldContainer = Fixture.Manager(Fixture.SpawnOwner(), 1);
    UInventoryManagerComponent* OtherPlayer = Fixture.Manager(Fixture.SpawnOwner(), 0, {Slot("Backpack", EInventoryEquipmentSlotKind::Equipment)});
    ABattery* OtherBackpack = Fixture.SpawnItem();
    UInventoryManagerComponent* OtherBag = Fixture.Manager(OtherBackpack, 1);
    UInventorySubsystem* Subsystem = Fixture.World->GetSubsystem<UInventorySubsystem>();
    FInventoryManagerVicinityCheckParameters Query;
    TArray<UInventoryManagerComponent*> Nearby;

    TestTrue(TEXT("player equips backpack"), Root->TryEquipItem(Backpack));
    TestTrue(TEXT("backpack equips nested container"), Bag->TryEquipItem(NestedItem));
    TestTrue(TEXT("other player equips backpack"), OtherPlayer->TryEquipItem(OtherBackpack));
    for (UInventoryManagerComponent* Inventory : {Root, Bag, Nested, WorldContainer, OtherPlayer, OtherBag})
        Subsystem->RegisterInventoryManager(Inventory);
    Query.IgnoredActors.Add(Player);
    TestFalse(TEXT("recursive ignore is opt-in"), Query.bAutoIgnoreRecursive);
    Nearby = Subsystem->GetInventoryManagersNearby(Player->GetActorLocation(), 1000.0f, Query);
    TestFalse(TEXT("generic query ignores explicit player"), Nearby.Contains(Root));
    TestTrue(TEXT("generic query retains equipped backpack by default"), Nearby.Contains(Bag));
    TestTrue(TEXT("generic query retains nested equipment by default"), Nearby.Contains(Nested));
    Query.bAutoIgnoreRecursive = true;
    Nearby = Subsystem->GetInventoryManagersNearby(Player->GetActorLocation(), 1000.0f, Query);
    TestFalse(TEXT("player inventory excluded"), Nearby.Contains(Root));
    TestFalse(TEXT("equipped backpack inventory excluded"), Nearby.Contains(Bag));
    TestFalse(TEXT("nested equipped inventory excluded"), Nearby.Contains(Nested));
    TestTrue(TEXT("world container remains nearby"), Nearby.Contains(WorldContainer));
    TestTrue(TEXT("other player's equipment remains nearby"), Nearby.Contains(OtherBag));
    FindFProperty<FFloatProperty>(UInventoryManagerComponent::StaticClass(), "MaxVicinityItemDistance")->SetPropertyValue_InContainer(Root, 1000.0f);
    Root->UpdateInventoryManagersInVicinity();
    TestFalse(TEXT("widget vicinity cache excludes equipped backpack"), Root->GetInventoryManagersInVicinity().Contains(Bag));
    TestFalse(TEXT("widget vicinity cache excludes nested equipment"), Root->GetInventoryManagersInVicinity().Contains(Nested));
    TestTrue(TEXT("widget vicinity cache retains world container"), Root->GetInventoryManagersInVicinity().Contains(WorldContainer));
    Root->DropItem(Backpack);
    Nearby = Subsystem->GetInventoryManagersNearby(Player->GetActorLocation(), 1000.0f, Query);
    TestTrue(TEXT("dropped backpack becomes nearby"), Nearby.Contains(Bag));
    TestTrue(TEXT("dropped backpack's nested inventory becomes nearby"), Nearby.Contains(Nested));
    return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FInventoryAttachmentEntryTest,
    "FirstPersonModule.Inventory.AttachmentEntryBindings",
    EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FInventoryAttachmentEntryTest::RunTest(const FString& Parameters)
{
    using namespace InventoryRoutingTests;
    FWorldFixture Fixture;
    UInventoryManagerComponent* Inventory = Fixture.Manager(Fixture.SpawnOwner(), 0, {Slot("MainHand", EInventoryEquipmentSlotKind::MainHand)});
    UInventoryAttachmentComponent* Attachment = Inventory->GetEquipmentSlot("MainHand");
    ABattery* Item = Fixture.SpawnItem();
    TStrongObjectPtr<UInventoryAttachmentCompEntry> Entry(NewObject<UInventoryAttachmentCompEntry>());

    TestFalse(TEXT("unbound entry is safe and empty"), Entry->HasValidAttachedItem());
    Entry->SetAttachmentComponent(Attachment);
    TestEqual(TEXT("first attachment assignment succeeds"), Entry->AttachmentComponent, Attachment);
    TestFalse(TEXT("empty runtime slot is safe"), Entry->HasValidAttachedItem());
    TestTrue(TEXT("item equips into the runtime slot"), Inventory->PickupItem(Item, FInteractionContext(true)));
    TestTrue(TEXT("entry detects its equipped item"), Entry->HasValidAttachedItem());
    Entry->NativeOnListItemObjectSet(nullptr);
    TestNull(TEXT("null list item clears the old binding"), Entry->AttachmentComponent);
    TestFalse(TEXT("cleared entry is safe"), Entry->HasValidAttachedItem());
    Entry->NativeOnListItemObjectSet(Attachment);
    TestTrue(TEXT("list item binds the occupied runtime slot"), Entry->HasValidAttachedItem());
    Entry->NativeOnListItemObjectSet(Item);
    TestNull(TEXT("incompatible list item clears the old binding"), Entry->AttachmentComponent);

    APawn* Pawn = Fixture.World->SpawnActor<APawn>();
    APlayerController* Controller = Fixture.World->SpawnActor<APlayerController>();
    TStrongObjectPtr<ULocalPlayer> LocalPlayer(NewObject<ULocalPlayer>(GEngine));
    UInventoryManagerComponent* DelayedInventory = NewObject<UInventoryManagerComponent>(Pawn);
    TStrongObjectPtr<UInventoryAttachmentCompEntry> DelayedEntry(NewObject<UInventoryAttachmentCompEntry>());

    DelayedInventory->EquipmentSlotConfigs = {Slot("Underwear", EInventoryEquipmentSlotKind::Equipment)};
    DelayedInventory->RegisterComponent();
    DelayedEntry->bAutoInitialize = true;
    DelayedEntry->AttachmentID = "Underwear";
    DelayedEntry->NativeOnInitialized();
    TestNull(TEXT("early initialization tolerates missing owning player"), DelayedEntry->AttachmentComponent);
    Controller->Player = LocalPlayer.Get();
    LocalPlayer->PlayerController = Controller;
    Controller->SetPawn(Pawn);
    DelayedEntry->SetPlayerContext(FLocalPlayerContext(LocalPlayer.Get(), Fixture.World));
    DelayedEntry->NativeTick(FGeometry(), 0.01f);
    TestNull(TEXT("automatic entry waits for runtime slots"), DelayedEntry->AttachmentComponent);
    DelayedInventory->InitializeEquipmentSlots();
    DelayedEntry->NativeTick(FGeometry(), 0.01f);
    TestEqual(TEXT("automatic entry binds when its runtime slot becomes available"), DelayedEntry->AttachmentComponent, DelayedInventory->GetEquipmentSlot("Underwear"));
    return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FInventoryTeardownTest,
    "FirstPersonModule.Inventory.ComponentTeardown",
    EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FInventoryTeardownTest::RunTest(const FString& Parameters)
{
    using namespace InventoryRoutingTests;
    FWorldFixture Fixture;
    AActor* Owner = Fixture.SpawnOwner();
    UInventoryManagerComponent* Inventory = Fixture.Manager(Owner, 1);
    ABattery* Item = Fixture.SpawnItem();
    TStrongObjectPtr<UInteractionContextTestReceiver> Receiver(NewObject<UInteractionContextTestReceiver>());

    Inventory->OnInventoryUpdated.AddDynamic(Receiver.Get(), &UInteractionContextTestReceiver::RecordInventoryUpdated);
    TestTrue(TEXT("live inventory accepts the item"), Inventory->AddItem(Item));
    TestTrue(TEXT("live inventory still broadcasts updates"), Receiver->InventoryUpdateCount > 0);
    Receiver->InventoryUpdateCount = 0;

    Fixture.World->bIsTearingDown = true;
    Inventory->DestroyComponent();
    TestEqual(TEXT("teardown does not broadcast gameplay updates"), Receiver->InventoryUpdateCount, 0);
    TestEqual(TEXT("teardown does not perform a gameplay drop"), Item->GetItemComponent()->GetItemPresence(), EItemPresence::Storage);
    TestEqual(TEXT("teardown leaves item ownership untouched"), Item->GetOwner(), Owner);
    Fixture.World->bIsTearingDown = false;

    // Reproduce late destruction with a valid owning actor but no world, as in the PIE crash.
    TStrongObjectPtr<AActor> WorldlessOwner(NewObject<AActor>());
    UInventoryManagerComponent* WorldlessInventory = NewObject<UInventoryManagerComponent>(WorldlessOwner.Get());

    WorldlessInventory->OnInventoryUpdated.AddDynamic(Receiver.Get(), &UInteractionContextTestReceiver::RecordInventoryUpdated);
    WorldlessInventory->ProcessEvent(WorldlessInventory->FindFunctionChecked("NativeOnInventoryUpdated"), nullptr);
    TestEqual(TEXT("worldless inventory does not broadcast updates"), Receiver->InventoryUpdateCount, 0);
    WorldlessInventory->OnComponentCreated();
    WorldlessInventory->ConditionalBeginDestroy();
    TestTrue(TEXT("late component destruction completes without accessing world subsystems"), WorldlessInventory->HasAnyFlags(RF_BeginDestroyed));
    return true;
}

struct FInventoryHoldState
{
    TSharedPtr<InventoryRoutingTests::FWorldFixture> Fixture;
    FAutomationTestBase* Test = nullptr;
    TStrongObjectPtr<UInteractionContextTestReceiver> Receiver;
    AFirstPersonCharacter* Player = nullptr;
    ABattery* Item = nullptr;
    UInteractiveManagerComponent* Input = nullptr;
    UInteractiveComponent* Interactive = nullptr;
    UInteractiveAction* Action = nullptr;
};

DEFINE_LATENT_AUTOMATION_COMMAND_ONE_PARAMETER(FCheckInventoryLongPress, TSharedPtr<FInventoryHoldState>, State);
bool FCheckInventoryLongPress::Update()
{
    // Advance game time without actor ticks, then explicitly exercise the manager tick.
    State->Fixture->World->Tick(LEVELTICK_TimeOnly, 0.1f);
    State->Input->TickComponent(0.1f, LEVELTICK_All, nullptr);
    State->Test->TestFalse(TEXT("tick before threshold does not start pickup"), State->Interactive->IsActionInProgress(State->Action));
    State->Fixture->World->Tick(LEVELTICK_TimeOnly, 0.2f);
    State->Input->TickComponent(0.2f, LEVELTICK_All, nullptr);
    State->Test->TestTrue(TEXT("threshold classifies the press as long"), State->Input->GetInteractionContext(State->Interactive, State->Action).bLongHeld);
    State->Test->TestTrue(TEXT("long press starts its longer animation duration"),
        FMath::IsNearlyEqual(State->Interactive->ActiveInteractionDuration, 0.5f));
    State->Test->TestTrue(TEXT("Started delegate receives long intent"), State->Receiver->Started.bLongHeld);
    State->Input->TickComponent(0.15f, LEVELTICK_All, nullptr);
    State->Test->TestTrue(TEXT("continued hold keeps the same action active"), State->Interactive->IsActionInProgress(State->Action));
    State->Input->EndInteractionPress();
    State->Test->TestTrue(TEXT("releasing long press does not trigger a second short interaction"),
        State->Interactive->IsActionInProgress(State->Action));
    State->Test->TestEqual(TEXT("item untouched until animation completes"),
        State->Item->GetItemComponent()->GetItemPresence(), EItemPresence::World);
    State->Interactive->NativeOnStopInteraction(State->Player, State->Action, EInteractionEndReason::Completed, State->Input->GetInteractionContext(State->Interactive, State->Action));
    State->Test->TestTrue(TEXT("completion retains long intent after manager clears context"), State->Receiver->Ended.bLongHeld);
    State->Test->TestFalse(TEXT("successful completion is not interrupted"), State->Receiver->Ended.bInterrupted);
    State->Test->TestEqual(TEXT("completed long press equips to hand"),
        State->Player->GetInventoryManager()->GetEquippedItem(), static_cast<AInventoryItem*>(State->Item));
    State->Test->TestEqual(TEXT("equipped item is not stored"),
        State->Player->GetInventoryManager()->GetInventorySlots().Num(), 0);
    State->Player->GetInventoryManager()->DropItem(State->Item);
    State->Input->BeginInteractionPress();
    State->Fixture->World->Tick(LEVELTICK_TimeOnly, 0.3f);
    State->Input->EndInteractionPress();
    State->Test->TestTrue(TEXT("release after threshold between manager ticks is still long"),
        State->Input->GetInteractionContext(State->Interactive, State->Action).bLongHeld);
    State->Interactive->NativeOnStopInteraction(State->Player, State->Action, EInteractionEndReason::Canceled, State->Input->GetInteractionContext(State->Interactive, State->Action));
    State->Test->TestEqual(TEXT("canceled release leaves the item in the world"),
        State->Item->GetItemComponent()->GetItemPresence(), EItemPresence::World);
    // Manual animation notifies must dispatch completion/cancellation, not just clear ActiveUser.
    FInteractiveActionConfig ManualConfig(UPickupAction::StaticClass());
    ManualConfig.InteractionType = EInteractionType::Manual;
    UInteractiveAction* Manual = NewObject<UPickupAction>(State->Interactive);
    Manual->Initialize(State->Interactive, ManualConfig);
    State->Interactive->RuntimeActions.Add(Manual);
    State->Input->StartInteraction(State->Interactive, Manual, FInteractionContext(true));
    State->Test->TestTrue(TEXT("manual pickup stays active"), State->Interactive->IsActionInProgress(Manual));
    State->Test->TestTrue(TEXT("manual pickup does not auto-complete on long duration"),
        FMath::IsNearlyZero(State->Interactive->ActiveInteractionDuration));
    struct FNotifyParameters { AFirstPersonCharacter* User; const UInteractiveAction* Action; };
    FNotifyParameters Notify{State->Player, Manual};
    State->Interactive->ProcessEvent(State->Interactive->FindFunctionChecked("CompleteInteraction"), &Notify);
    State->Test->TestEqual(TEXT("manual completion performs pickup"),
        State->Player->GetInventoryManager()->GetEquippedItem(), static_cast<AInventoryItem*>(State->Item));
    State->Player->GetInventoryManager()->DropItem(State->Item);
    State->Input->StartInteraction(State->Interactive, Manual, FInteractionContext(true));
    State->Interactive->ProcessEvent(State->Interactive->FindFunctionChecked("CancelInteraction"), &Notify);
    State->Test->TestEqual(TEXT("manual cancellation leaves item in world"),
        State->Item->GetItemComponent()->GetItemPresence(), EItemPresence::World);
    State->Test->TestNull(TEXT("manual cancellation leaves hand empty"),
        State->Player->GetInventoryManager()->GetEquippedItem());
    State->Test->TestTrue(TEXT("explicit cancellation retains long intent"), State->Receiver->Ended.bLongHeld);
    State->Test->TestFalse(TEXT("explicit cancellation is not an interruption"), State->Receiver->Ended.bInterrupted);

    FInteractionContext StartContext(true);
    StartContext.bInterrupted = true; // A new attempt must clear terminal flags.
    State->Input->StartInteraction(State->Interactive, Manual, StartContext);
    State->Test->TestFalse(TEXT("start sanitizes the interruption flag"), State->Receiver->Started.bInterrupted);
    FInteractionContext StopContext; // Stop cannot change the accepted long-press intent.
    StopContext.bInterrupted = true;
    State->Input->StopInteraction(State->Interactive, Manual, StopContext);
    State->Test->TestTrue(TEXT("interruption retains the accepted long intent"), State->Receiver->Ended.bLongHeld);
    State->Test->TestTrue(TEXT("interruption reaches end context"), State->Receiver->Ended.bInterrupted);
    State->Test->TestEqual(TEXT("end reason matches interruption context"), State->Receiver->EndReason, EInteractionEndReason::Interrupted);
    State->Test->TestEqual(TEXT("identity remains separate from context"), State->Receiver->LastAction, static_cast<const UInteractiveAction*>(Manual));
    State->Test->TestFalse(TEXT("manager clears completed context"), State->Input->GetInteractionContext(State->Interactive, Manual).bLongHeld);

    State->Input->StartInteraction(State->Interactive, Manual, FInteractionContext());
    State->Test->TestFalse(TEXT("next short attempt does not inherit long intent"), State->Receiver->Started.bLongHeld);
    State->Input->StopInteraction(State->Interactive, Manual, FInteractionContext());
    State->Test->TestFalse(TEXT("short cancellation context is not long"), State->Receiver->Ended.bLongHeld);
    State->Test->TestFalse(TEXT("next cancellation does not inherit interruption"), State->Receiver->Ended.bInterrupted);
    return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FInventoryPressTest,
    "FirstPersonModule.Inventory.PressIntentAndCompletionRevalidation",
    EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FInventoryPressTest::RunTest(const FString& Parameters)
{
    using namespace InventoryRoutingTests;
    TSharedPtr<FInventoryHoldState> State = MakeShared<FInventoryHoldState>();
    State->Fixture = MakeShared<FWorldFixture>();
    State->Test = this;
    State->Player = State->Fixture->World->SpawnActor<AFirstPersonCharacter>();
    UPlayerInventoryManagerComponent* Inventory = State->Player->GetInventoryManager();
    Inventory->InitializeEquipmentSlots();
    Inventory->SlotCapacity = 1; // A generic local container for this input test.
    State->Item = State->Fixture->SpawnItem();
    State->Input = State->Player->GetInteractionsManager();
    State->Interactive = State->Item->InteractiveComponent;
    State->Receiver.Reset(NewObject<UInteractionContextTestReceiver>());
    State->Interactive->OnInteractionBegin.AddDynamic(State->Receiver.Get(), &UInteractionContextTestReceiver::RecordStart);
    State->Interactive->OnInteractionEnd.AddDynamic(State->Receiver.Get(), &UInteractionContextTestReceiver::RecordEnd);
    FInteractiveActionConfig Config(UPickupAction::StaticClass(), 0.1f);
    Config.LongHeldActionDuration = 0.5f;
    State->Action = NewObject<UPickupAction>(State->Interactive);
    State->Action->Initialize(State->Interactive, Config);
    State->Interactive->RuntimeActions.Add(State->Action);
    FindFProperty<FObjectProperty>(UInteractiveManagerComponent::StaticClass(), "FocusedInteractive")
        ->SetObjectPropertyValue_InContainer(State->Input, State->Interactive);
    FindFProperty<FObjectProperty>(UInteractiveManagerComponent::StaticClass(), "PendingAction")
        ->SetObjectPropertyValue_InContainer(State->Input, State->Action);
    // The fixture player has no local controller; seed its traced focus for manager ticks.
    FindFProperty<FArrayProperty>(UInteractiveManagerComponent::StaticClass(), "TracedInteractives")
        ->ContainerPtrToValuePtr<TArray<UInteractiveComponent*>>(State->Input)->Add(State->Interactive);
    State->Input->BeginInteractionPress();
    TestFalse(TEXT("button down does not start pickup before classification"),
        State->Interactive->IsActionInProgress(State->Action));
    State->Input->EndInteractionPress();
    TestFalse(TEXT("quick release is short press"), State->Input->GetInteractionContext(State->Interactive, State->Action).bLongHeld);
    TestTrue(TEXT("short press starts storage animation"), State->Interactive->IsActionInProgress(State->Action));
    ABattery* Filler = State->Fixture->SpawnItem();
    TestTrue(TEXT("another item can take the remaining capacity"), Inventory->PickupItem(Filler, FInteractionContext(false)));
    State->Interactive->NativeOnStopInteraction(State->Player, State->Action, EInteractionEndReason::Completed, State->Input->GetInteractionContext(State->Interactive, State->Action));
    TestEqual(TEXT("completion rechecks space and leaves original item untouched"),
        State->Item->GetItemComponent()->GetItemPresence(), EItemPresence::World);
    TestNull(TEXT("failed completion leaves original owner unchanged"), State->Item->GetOwner());
    Inventory->DropItem(Filler);
    State->Input->BeginInteractionPress();
    ADD_LATENT_AUTOMATION_COMMAND(FCheckInventoryLongPress(State));
    return true;
}
#endif
