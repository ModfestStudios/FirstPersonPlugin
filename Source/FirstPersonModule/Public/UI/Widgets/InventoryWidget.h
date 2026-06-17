// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Inventory/Inventory.h"
#include "InventoryWidget.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnNearbyManagersUpdate,TArray<class UInventoryManagerComponent*>,NearbyManagers);

/**
 * 
 */
UCLASS(abstract)
class FIRSTPERSONMODULE_API UInventoryWidget : public UUserWidget
{
	GENERATED_BODY()
public:


private:



	class UInventorySubsystem* CachedInventorySubsystem;

	/*events*/
public:


	//===============================================================================================================================================
	//===================================================================FUNCTIONS===================================================================
	//===============================================================================================================================================
	
public:

	UInventoryWidget(const FObjectInitializer& ObjectInitializer);

	
	UFUNCTION()
		virtual void InitializeInventoryWidget(class UInventoryManagerComponent* InventoryManagerComponent);

	virtual void NativeOnInitialized() override;
	virtual void NativeConstruct() override;
	virtual void NativeDestruct();

	virtual void SynchronizeProperties() override;

	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;


	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Inventory Widget")
		class UInventoryManagerComponent* GetInventoryManager();

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Inventory Widget")
		class AFirstPersonCharacter* GetOwningCharacter();


	UFUNCTION()
		virtual void OnInventoryManagerOpened();
	UFUNCTION(BlueprintImplementableEvent, Category = "Inventory Widget", meta = (DisplayName = "On Inventory Manager Opened"))
		void BP_OnInventoryManagerOpened();

	UFUNCTION()
		virtual void OnInventoryManagerClosed();
	UFUNCTION(BlueprintImplementableEvent, Category = "Inventory Widget", meta = (DisplayName = "On Inventory Manager Closed"))
		void BP_OnInventoryManagerClosed();

	UFUNCTION()
		void NativeOnInventoryManagersInVicinityUpdated();
	UFUNCTION(BlueprintImplementableEvent, Category = "Inventory Widget", meta = (DisplayName = "On Inventory Managers In Vicinity Updated"))
		void BP_OnInventoryManagersInVicinityUpdated();
	UFUNCTION()
		void NativeOnInventoryItemsInVicinityUpdated();
	UFUNCTION(BlueprintImplementableEvent, Category = "Inventory Widget", meta = (DisplayName = "On Inventory Items In Vicinity Updated"))
		void BP_OnInventoryItemsInVicinityUpdated();


	/*called automatically when the Inventory Subsystem's "Selected Inventory Manager" changes or is updated*/
	UFUNCTION()
		void OnSelectedInventoryManagerUpdated();
	/*called automatically when the Inventory Subsystem's "Selected Inventory Manager" changes or is updated*/
	UFUNCTION(BlueprintImplementableEvent, Category = "Inventory Widget", meta = (DisplayName = "On Selected Inventory Manager Updated"))
		void BP_OnSelectedInventoryManagerUpdated();


	UFUNCTION(BlueprintCallable, Category = "Inventory")
		virtual void CloseInventory();


	UFUNCTION()
		virtual void OnInventoryViewChanged(EInventoryView NewInventoryView);
	UFUNCTION(BlueprintImplementableEvent, Category = "Inventory Widget", meta = (DisplayName = "On Inventory View Changed"))
		void BP_OnInventoryViewChanged(EInventoryView NewInventoryView);


	//===================
	//=====UTILITIES=====
	//===================
	
	class UInventorySubsystem* GetInventorySubsystem();
	
};
