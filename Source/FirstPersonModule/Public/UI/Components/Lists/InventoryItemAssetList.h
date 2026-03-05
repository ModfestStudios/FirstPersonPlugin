// copyrighted 2024-2025 - Modfest Studios & Micah A. Parker

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Components/ListView.h"
#include "Components/ListViewBase.h"
#include "Widgets/Views/SListView.h"
#include "InventoryItemAssetList.generated.h"

UENUM(BlueprintType)
enum class EInventorySelectionType : uint8
{
	PrimaryWeapon,
	SecondaryWeapon,
	AlternativeWeapon,
	Inventory
};

/**
 * 
 */
UCLASS()
class FIRSTPERSONMODULE_API UInventoryItemAssetList : public UUserWidget
{
	GENERATED_BODY()
public:

	UPROPERTY(meta = (BindWidget))
		UListView* LIST_ItemList;

	bool bClearingSelection = false;

	/*set a UInventoryConfig (Data Asset) here and the system will automatically sync the list with the Items property*/
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Inventory Items")
		class UInventoryConfig* ItemConfig;
	/*will automatically set selection based on*/
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Inventroy Items")
		EInventorySelectionType SelectionType;
	

	UPROPERTY(BlueprintReadOnly, Category = "Inventory Items")
		TArray<class UInventoryItemAsset*> Items;


	//===================================================================================================================================
	//=============================================================FUNCTIONS=============================================================
	//===================================================================================================================================

	virtual void NativeConstruct() override;
	UFUNCTION()
		virtual void OnSelectionChanged(UObject* Item);
	UFUNCTION(BlueprintCallable, Category = "Inventory Items")
		virtual void ClearSelection();



	//============================
	//=========INITIATION=========
	//============================

	/*generates the ItemList from the ItemConfig*/
	UFUNCTION(BlueprintCallable, Category = "Inventory Items")
		virtual void InitItemListFromConfig();
	/*generates the ItemList from provided items*/
	UFUNCTION(BlueprintCallable, Category = "Inventory Items")
		virtual void InitItemList(TArray<class UInventoryItemAsset*> ItemList);

	/*call this to trigger a refreshing of the Items list*/
	UFUNCTION(BlueprintCallable, Category = "Inventory Items")
		virtual void RefreshItemList();
	UFUNCTION(BlueprintImplementableEvent, Category = "Inventory Items", meta = (DisplayName = "On Item List Updated"))
		void BP_OnItemListUpdated();


	/*auto-selects the appropriate item from what the character listed*/
	UFUNCTION(BlueprintCallable, Category = "Inventory Items")
		virtual void DefaultItemSelectionFromCharacterLoadout();
		
	
	UFUNCTION(BlueprintCallable, Category = "Inventory Items")
		virtual void SetPrimaryWeapon(TSubclassOf<class AActor> WeaponClass);
	UFUNCTION(BlueprintCallable, Category = "Inventory Items")
		virtual void ClearPrimaryWeapon();
	UFUNCTION(BlueprintCallable, Category = "Inventory Items")
		virtual void SetSecondaryWeapon(TSubclassOf<class AActor> WeaponClass);
	UFUNCTION(BlueprintCallable, Category = "Inventory Items")
		virtual void ClearSecondaryWeapon();
	UFUNCTION(BlueprintCallable, Category = "Inventory Items")
		virtual void SetAlternativeWeapon(TSubclassOf<class AActor> WeaponClass);
	UFUNCTION(BlueprintCallable, Category = "Inventory Items")
		virtual void ClearAlternativeWeapon();

	///==========================
	//=========UTILITIES=========
	///==========================

public:
	/*grabs the Character's Loadout from CharacterSubsystem or MissionSubsystem*/
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Inventory Items")
		class AInventoryLoadout* GetSelectedCharacterLoadout();
};
