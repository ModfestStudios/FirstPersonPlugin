// Copyright (c) 2022 Pocket Sized Animations

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Inventory/InventoryItem.h"
#include "Interfaces/InteractiveActorInterface.h"
#include "Weapon.generated.h"

UCLASS(abstract)
class FIRSTPERSONMODULE_API AWeapon : public AInventoryItem, public IInteractiveActorInterface
{
	GENERATED_BODY()
public:

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Third Person")
		USkeletalMeshComponent* Mesh;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "First Person")
		class UFirstPersonItemComponent* FirstPersonComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Inventory")
		class UInventoryItemComponent* ItemComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Inventory")
		class UInteractiveCollisionComponent* InteractionCollision;

public:
	// Sets default values for this actor's properties
	AWeapon(const FObjectInitializer& ObjectInitializer);

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;


public:
	UFUNCTION(BlueprintCallable, Category = "Weapons")
		virtual void BeginFire();
	UFUNCTION(BlueprintCallable, Category = "Firing")
		virtual void EndFire();


	//=================================
	//============EQUIPPING============
	//=================================
public:
	/*Inventory Item interface stuff*/
	virtual void OnEquip_Implementation() override;
	virtual void OnUnequip_Implementation() override;
	virtual void OnPickedUp_Implementation() override;

	//===================================
	//============ATTACHMENTS============
	//===================================
	UFUNCTION(BlueprintCallable, Category = "Attachments")
		virtual void AttachFirstPersonMesh();
	UFUNCTION(BlueprintCallable, Category = "Attachments")
		virtual void AttachThirdPersonMesh();

	//==================================
	//============ANIMATIONS============
	//==================================
	/*AnimNotify functions are used by Animations in Blueprints to update the code-behind of the states its hit*/
	UFUNCTION()
		virtual void OnAnimNotify_AttachFirstPersonMesh();
	UFUNCTION()
		virtual void OnAnimNotify_AttachThirdPersonMesh();

	//===============================
	//==========INTERACTION==========
	//===============================
public:
	
		//virtual void OnInteraction_Implementation(class AFirstPersonCharacter* User, class UInteractiveAction* Action) override;	
		//virtual bool IsInteractionAvailable_Implementation(class AFirstPersonCharacter* User, class UInteractiveAction* Action) override;



	//===============================
	//===========COLLISION===========
	//===============================
	UFUNCTION()
		virtual void DisableCollision();


	//=============================
	//==========OWNERSHIP==========
	//=============================
public:
	UFUNCTION()
		bool IsLocallyControlled();
	UFUNCTION()
		APawn* GetOwningPawn();
	UFUNCTION()
		APlayerController* GetOwningPlayerController();

};
