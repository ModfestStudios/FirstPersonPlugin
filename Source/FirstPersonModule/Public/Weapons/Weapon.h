// Copyright (c) 2022 Pocket Sized Animations

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Inventory/InventoryItem.h"
#include "Weapon.generated.h"

UCLASS(abstract)
class FIRSTPERSONMODULE_API AWeapon : public AInventoryItem
{
	GENERATED_BODY()
public:

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Third Person")
		class USkeletalMeshComponent* Mesh;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "First Person")
		class UFirstPersonItemComponent* FirstPersonComponent;


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
