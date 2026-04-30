// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Inventory/Inventory.h"
#include "GameFramework/Actor.h"
#include "Interfaces/InventoryItemInterface.h"
#include "InventoryItem.generated.h"


UCLASS(abstract)
class FIRSTPERSONMODULE_API AInventoryItem : public AActor, public IInventoryItemInterface
{
	GENERATED_BODY()
public:


	//====ATTACHMENTS====
protected:
	UPROPERTY()
		bool bAttached = false;


public:
	FIntPoint RegisteredGridKey;


	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Inventory")
		class UInventoryItemComponent* ItemComponent;

	//=====================================================================================================================================
	//==============================================================FUNCTIONS==============================================================
	//=====================================================================================================================================

	virtual void OnEquip_Implementation() override;
	virtual void OnUnequip_Implementation() override;
	virtual void OnPickedUp_Implementation() override;


public:	
	// Sets default values for this actor's properties
	AInventoryItem(const FObjectInitializer& ObjectInitializer);

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;


	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Inventory Item")
		FText GetItemName();
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Inventory Item")
		UMaterialInterface* GetItemIcon();

	

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Inventory Item")
		EItemSize GetItemSize();
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Inventory Item")
		EItemPresence GetItemPresence();


	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Inventory Item")
		virtual bool CanBePickedUp();


	UFUNCTION()
		virtual void OnItemStored(class AActor* StorageActor);
	UFUNCTION()
		virtual void OnItemPickedUp(class AActor* ReceivingActor);
	UFUNCTION()
		virtual void OnItemDropped(class AActor* RejectingActor);





	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Inventory Item")
		class UInventoryItemComponent* GetItemComponent();




	//=======================
	//======ATTACHMENTS======
	//=======================
public:
	/*called whenever an item is attached to us*/
	UFUNCTION()
		virtual void OnItemAttached(class AInventoryItem* Item, class UInventoryAttachmentComponent* AttachedComponent);

	//=====================
	//=======MANAGER=======
	//=====================
public:
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Inventory Manager")
		class UInventoryManagerComponent* GetInventoryManager();

	//====================
	//========GRID========
	//====================

	UFUNCTION()
		void UpdateGridKey(FIntPoint NewGridKey);

};
