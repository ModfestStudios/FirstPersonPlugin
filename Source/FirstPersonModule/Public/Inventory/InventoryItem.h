// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Inventory/Inventory.h"
#include "GameFramework/Actor.h"
#include "Interfaces/InventoryItemInterface.h"
#include "Interfaces/InteractiveActorInterface.h"
#include "InventoryItem.generated.h"


UCLASS(abstract)
class FIRSTPERSONMODULE_API AInventoryItem : public AActor, public IInventoryItemInterface, public IInteractiveActorInterface
{
	GENERATED_BODY()
public:


	//====ATTACHMENTS====
protected:
	UPROPERTY()
		bool bAttached = false;


public:
	FIntPoint RegisteredGridKey;


	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Interactive")
		class UInteractiveComponent* InteractiveComponent;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Inventory")
		class UInventoryItemComponent* ItemComponent;

	//=====================================================================================================================================
	//==============================================================FUNCTIONS==============================================================
	//=====================================================================================================================================


public:	
	// Sets default values for this actor's properties
	AInventoryItem(const FObjectInitializer& ObjectInitializer);

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;



	//===================================================
	//====================INTERACTION====================
	//===================================================

protected:
	virtual void NativeOnInteractionStarted(class AFirstPersonCharacter* User, class UInteractiveComponent* Interactive, const class UInteractiveAction* Action, const FInteractionContext& Context) override;
	virtual void NativeOnInteractionCompleted(class AFirstPersonCharacter* User, class UInteractiveComponent* Interactive, const class UInteractiveAction* Action, const FInteractionContext& Context) override;
	virtual void NativeOnInteractionCanceled(class AFirstPersonCharacter* User, class UInteractiveComponent* Interactive, const class UInteractiveAction* Action, const FInteractionContext& Context) override;
	virtual bool IsActionAvailable_Implementation(const AFirstPersonCharacter* User, const UInteractiveComponent* Interactive, const UInteractiveAction* Action) const override;

public:
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Inventory Item")
		FText GetItemName();
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Inventory Item")
		UMaterialInterface* GetItemIcon();

	

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Inventory Item")
		EItemSize GetItemSize();
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Inventory Item")
		EItemPresence GetItemPresence();


	virtual bool CanBePickedUp_Implementation() const override;


protected:
	virtual void NativeOnItemStored(AActor* StorageActor) override;
    virtual void NativeOnEquippingCompleted(AFirstPersonCharacter* User, UInventoryItemComponent* Component, UInventoryManagerComponent* Manager) override;
	virtual void NativeOnItemPickedUp(AActor* ReceivingActor, const FInteractionContext& Context) override;
	virtual void NativeOnItemDropped(AActor* RejectingActor) override;





public:
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
