// Fill out your copyright notice in the Description page of Project Settings.


#include "Components/PlayerInventoryManagerComponent.h"

#include "Inventory/InventoryEquipmentSlot.h"

/*items*/
#include "Equipment/Backpack.h"
#include "Clothing/Glasses.h"
#include "Clothing/Gloves.h"
#include "Clothing/Jacket.h"
#include "Equipment/Mask.h"
#include "Clothing/Pants.h"
#include "Clothing/Shirt.h"
#include "Clothing/Shoes.h"
#include "Clothing/Underwear.h"
#include "Clothing/Undershirt.h"
#include "Equipment/Vest.h"
#include "Equipment/Watch.h"
#include "Clothing/Headwear.h"

UPlayerInventoryManagerComponent::UPlayerInventoryManagerComponent()
{
    SlotCapacity = 0;
    auto AddSlot = [this](FName ID, TSubclassOf<AInventoryItem> Type,
                         EInventoryEquipmentSlotKind Kind = EInventoryEquipmentSlotKind::Equipment)
    {
        FInventoryEquipmentSlotConfig Config;

        Config.SlotID = ID;
        Config.DisplayName = FText::FromName(ID);
        Config.AcceptedItemClass = Type;
        Config.SlotKind = Kind;
        EquipmentSlotConfigs.Add(Config);
    };
    AddSlot("Headwear", AHeadwear::StaticClass());
    AddSlot("Vest", AVest::StaticClass());
    AddSlot("Backpack", ABackpack::StaticClass());
    AddSlot("Glasses", AGlasses::StaticClass());
    AddSlot("Gloves", AGloves::StaticClass());
    AddSlot("Jacket", AJacket::StaticClass());
    AddSlot("Shirt", AShirt::StaticClass());
    AddSlot("Pants", APants::StaticClass());
    AddSlot("Shoes", AShoes::StaticClass());
    AddSlot("Undershirt", AUndershirt::StaticClass());
    AddSlot("Underwear", AUnderwear::StaticClass());
    AddSlot("Mask", AMask::StaticClass());
    AddSlot("Watch", AWatch::StaticClass());
    AddSlot("MainHand", nullptr, EInventoryEquipmentSlotKind::MainHand);
    // OffHand is opt-in through EquipmentSlotConfigs.
}
