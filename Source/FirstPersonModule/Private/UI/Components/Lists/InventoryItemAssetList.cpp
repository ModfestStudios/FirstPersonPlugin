// copyrighted 2024-2025 - Modfest Studios & Micah A. Parker


#include "UI/Components/Lists/InventoryItemAssetList.h"
#include "Inventory/InventoryItemAsset.h"
#include "Inventory/InventoryConfig.h"
#include "Inventory/InventoryLoadout.h"
#include "Subsystems/CharacterSubsystem.h"
#include "Subsystems/MissionSubsystem.h"

/*engine*/
#include "Engine/GameInstance.h"

void UInventoryItemAssetList::NativeConstruct()
{
	Super::NativeConstruct();

	if (LIST_ItemList)
	{
		LIST_ItemList->OnItemSelectionChanged().AddUObject(this, &UInventoryItemAssetList::OnSelectionChanged);
	}
}

void UInventoryItemAssetList::OnSelectionChanged(UObject* Item)
{
	if (!LIST_ItemList)
		return;


	if (Item)
	{
		/*load asset*/
		bool bIsSelected = LIST_ItemList->IsItemSelected(Item);

		UInventoryItemAsset* ItemAsset = Cast<UInventoryItemAsset>(Item);
		if (ItemAsset && !ItemAsset->ItemClass.IsValid() && !ItemAsset->ItemClass.IsNull())
			ItemAsset->ItemClass.LoadSynchronous();

		TSubclassOf<AActor> ItemClass = (ItemAsset && ItemAsset->ItemClass.IsValid()) ? ItemAsset->ItemClass.Get() : nullptr;

		if (bIsSelected && ItemClass)
		{
			switch (SelectionType)
			{

			case EInventorySelectionType::PrimaryWeapon:
				SetPrimaryWeapon(ItemClass);
				break;
			case EInventorySelectionType::SecondaryWeapon:
				SetSecondaryWeapon(ItemClass);
				break;
			case EInventorySelectionType::AlternativeWeapon:
				SetAlternativeWeapon(ItemClass);
				break;
			default:
				break;
			}
		}
	}

	else if(!bClearingSelection) //only make an update to the player's inventory if we're not calling a clear command for UI refresh
	{
		switch (SelectionType)
		{
			case EInventorySelectionType::PrimaryWeapon:
				ClearPrimaryWeapon();
				break;
			case EInventorySelectionType::SecondaryWeapon:
				ClearSecondaryWeapon();
				break;
			case EInventorySelectionType::AlternativeWeapon:
				ClearAlternativeWeapon();
				break;
			default:
				break;
		}
	}	

	bClearingSelection = false;
}

void UInventoryItemAssetList::ClearSelection()
{
	bClearingSelection = true;

	if (LIST_ItemList)
		LIST_ItemList->ClearSelection();
}


//============================
//=========INITIATION=========
//============================

void UInventoryItemAssetList::InitItemListFromConfig()
{
	if (!ItemConfig)
		return;

	Items.Empty();

	for (const TSoftObjectPtr<UInventoryItemAsset>& SoftAsset : ItemConfig->Items)
	{
		if (SoftAsset.IsNull() == false)
		{
			UInventoryItemAsset* Asset = SoftAsset.LoadSynchronous();
			if (Asset)
			{
				Items.Add(Asset);
			}
		}
	}

	RefreshItemList();
}

void UInventoryItemAssetList::InitItemList(TArray<class UInventoryItemAsset*> ItemList)
{
	Items.Empty();
	Items = ItemList;	
	RefreshItemList();	
}

void UInventoryItemAssetList::RefreshItemList()
{	
	LIST_ItemList->ClearListItems();

	for (UInventoryItemAsset* ItemAsset : Items)
	{
		if (IsValid(ItemAsset))
			LIST_ItemList->AddItem(ItemAsset);
	}		

	
	BP_OnItemListUpdated();
}

void UInventoryItemAssetList::DefaultItemSelectionFromCharacterLoadout()
{
	if (AInventoryLoadout* Loadout = GetSelectedCharacterLoadout())
		{
			TSoftClassPtr<AActor> ItemClass;

			switch (SelectionType)
			{
				case  EInventorySelectionType::PrimaryWeapon:
					ItemClass = Loadout->PrimaryWeapon.ItemClass;
					break;
				case EInventorySelectionType::SecondaryWeapon:
					ItemClass = Loadout->SecondaryWeapon.ItemClass;
					break;
				case EInventorySelectionType::AlternativeWeapon:
					ItemClass = Loadout->AlternativeWeapon.ItemClass;
					break;
				default:
					break;
			}

			if (ItemClass)
			{
				//LIST_ItemList->SetItemSelection(ItemAsset, true);
				for (auto Item : LIST_ItemList->GetListItems())
				{
					if (UInventoryItemAsset* ListItemAsset = Cast<UInventoryItemAsset>(Item))
					{
						if (ItemClass == ListItemAsset->ItemClass)
							LIST_ItemList->SetItemSelection(ListItemAsset, true);
					}
				}
			}
		}
}

void UInventoryItemAssetList::SetPrimaryWeapon(TSubclassOf<class AActor> WeaponClass)
{
	AInventoryLoadout* CharacterLoadout = GetSelectedCharacterLoadout();
	FInventoryItemSpawnParams SpawnParams;
	SpawnParams.ItemClass = WeaponClass;


	if (CharacterLoadout)
		CharacterLoadout->SetPrimaryWeapon(SpawnParams);
	
}

void UInventoryItemAssetList::ClearPrimaryWeapon()
{
	AInventoryLoadout* CharacterLoadout = GetSelectedCharacterLoadout();

	if (CharacterLoadout)
		CharacterLoadout->ClearPrimaryWeapon();
}

void UInventoryItemAssetList::SetSecondaryWeapon(TSubclassOf<class AActor> WeaponClass)
{
	AInventoryLoadout* CharacterLoadout = GetSelectedCharacterLoadout();
	FInventoryItemSpawnParams SpawnParams;
	SpawnParams.ItemClass = WeaponClass;

	if (CharacterLoadout)
		CharacterLoadout->SetSecondaryWeapon(SpawnParams);
}

void UInventoryItemAssetList::ClearSecondaryWeapon()
{
	AInventoryLoadout* CharacterLoadout = GetSelectedCharacterLoadout();

	if (CharacterLoadout)
		CharacterLoadout->ClearSecondaryWeapon();
}

void UInventoryItemAssetList::SetAlternativeWeapon(TSubclassOf<class AActor> WeaponClass)
{
	AInventoryLoadout* CharacterLoadout = GetSelectedCharacterLoadout();
	FInventoryItemSpawnParams SpawnParams;
	SpawnParams.ItemClass = WeaponClass;

	if (CharacterLoadout)
		CharacterLoadout->SetAlternativeWeapon(SpawnParams);
}

void UInventoryItemAssetList::ClearAlternativeWeapon()
{
	AInventoryLoadout* CharacterLoadout = GetSelectedCharacterLoadout();

	if (CharacterLoadout)
		CharacterLoadout->ClearAlternativeWeapon();
}

AInventoryLoadout* UInventoryItemAssetList::GetSelectedCharacterLoadout()
{	
	if (UCharacterSubsystem* CSS = GetGameInstance()->GetSubsystem<UCharacterSubsystem>())
	{
		if (ACharacterInfo* Char = CSS->GetSelectedCharacter())
			return Char->Loadout;
	}
	
	return nullptr;
}
