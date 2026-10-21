# Runtime inventory and equipment

Configure Equipment Slot Configs on an InventoryManagerComponent. The manager creates UInventoryAttachmentComponent instances at runtime on its owner; slots are not constructor subobjects. IDs must be unique. The player manager defaults to zero local capacity and a MainHand slot; OffHand is opt-in.

Each entry defines an ID, display name, accepted item class (subclasses are supported), Equipment/MainHand/OffHand kind, optional default actor class, attachment component/socket/transform, and storage search priority. Specialized slots require an accepted class; hands may accept any inventory item. Equipment actors own their own InventoryManagerComponent; set their local SlotCapacity in their asset defaults.

Short pickup presses search local storage, then the managers on directly equipped items. Long presses search compatible empty specialized slots on both levels before the receiving manager's hands. Searches never enter a third manager level. Occupied slots are not displaced. A rejected operation preserves the source item's location, owner, presence, and visibility.

Stored items are owned by the actual containing actor. Held/equipped items occupy an attachment slot and consume no storage capacity. PreviousStorage remembers the last storage manager; TryPutAwayItem tries it first if still accessible and able to fit the item, then normal routing. Moving/dropping a container preserves its child equipment and inventory.

LongPressThreshold on InteractiveManagerComponent defaults to 0.25 seconds. The manager checks pending pickup presses in Tick using elapsed world time; the existing 0.15-second tick interval determines when a held press is detected. Pickup begins on quick release or on the first tick after the threshold is reached (release after the threshold between ticks also counts as long), with FInteractionContext passed through interaction and pickup lifecycle hooks. The context contains bLongHeld and bInterrupted; User, Interactive, and Action remain separate parameters. The requesting InteractiveManagerComponent retains the accepted context for the active interaction, and completion/cancellation callbacks receive a snapshot before that state is cleared. InteractiveComponent does not store a separate press-intent flag. Action Configs controls short action duration and LongHeldActionDuration (default 1 second). Manual actions wait for CompleteInteraction/CancelInteraction; animation Blueprints can call these at their notify points. Releasing a classified pickup does not create another interaction.

NativeOnItemStored hides the base item and attaches it to the containing actor. NativeOnEquippingCompleted attaches using the actual slot and shows the item. NativeOnItemDropped detaches and shows it. Override these hooks for custom presentation; actor hooks also handle replicated presentation. bCompleteEquipAutomatically defaults true; disable it when custom equip logic will call EndEquip.

Default slot actors initialize before ordinary DefaultItems. AutoEquip defaults try equipment/hands first and then storage. Items that cannot fit are left in the world with a warning. Recursive default actor spawning is bounded.

## Migration and validation

The character's individual clothing/backpack/weapon attachment components are removed. Character equipment lookups now forward to the manager. Configure former per-component DefaultAttachment and socket settings in EquipmentSlotConfigs. UI equipment entries now reference runtime attachment components. Refresh/recompile existing Blueprint nodes whose native interface signatures or slot types changed. Interaction and pickup events now expose a Context pin instead of separate long-held/interrupted booleans; use Break Interaction Context to read these flags. Owner functions configured by name must also accept Context in their interaction event parameters.

Automation suites:
- FirstPersonModule.Inventory.NearbyManagerOwnership
- FirstPersonModule.Inventory.AttachmentEntryBindings
- FirstPersonModule.Inventory.ComponentTeardown
- FirstPersonModule.Inventory.RoutingAndAtomicTransfers
- FirstPersonModule.Inventory.PressIntentAndCompletionRevalidation

These exercise actual world actors, interface dispatch, and component destruction after the world is unavailable or tearing down. They do not replace visual animation checks or multiplayer PIE testing.

Blueprint validation completed for the plugin allow list. It reported existing missing-function errors in PlayerMasterAnimBlueprint (movement-direction getters and IsDying) and M4Proxy_AnimBlueprint (HasWeaponEquipped); these functions were absent before this migration. No Blueprint packages were saved.

Inventory attachment entry bindings should use HasValidAttachedItem rather than a pure Blueprint AND combining attachment validity with GetAttachedItem. Pure AND evaluates both inputs and can access a null attachment. List entry assignment supports the first binding and clears stale bindings when the incoming object is null or incompatible.

Automatic attachment entries retry at widget construction and on tick while unbound, allowing the owning pawn and runtime equipment slots to become available after widget initialization. Supply an Owning Player when creating the parent widget so child entries can resolve its pawn. List-managed entries retain explicit list bindings.

Nearby manager queries filter only explicit IgnoredActors by default. Setting bAutoIgnoreRecursive opts into expanding that list through equipment slots and nested equipped containers, with a cycle guard. The inventory widget's manager query adds its owner to IgnoredActors and enables this option. Direct Blueprint calls can choose the same behavior through VicinityCheckParameters. Dropped containers are no longer excluded.
