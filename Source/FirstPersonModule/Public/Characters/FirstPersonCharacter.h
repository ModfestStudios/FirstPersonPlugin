// Copyrighted Modfest Studios 2024

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Damage/DamageCondition.h"
#include "FirstPersonCharacter.generated.h"

/*collection of info used to replicate a death*/
USTRUCT(BlueprintType)
struct FDeathInfo
{
	GENERATED_USTRUCT_BODY()
public:

	UPROPERTY()
		UDamageType* DamageType = nullptr;
	UPROPERTY()
		AActor* DamageCauser = nullptr;
	UPROPERTY()
		AController* DamageInstigator = nullptr;
	/*pass a death animation in if you wish to override the Animation Blueprint*/
	UPROPERTY()
		UAnimMontage* DeathAnimation = nullptr;

	FDeathInfo() {}
	FDeathInfo(UDamageType* DamageType, AActor* DamageCauser, AController* DamageInstigator)
	{
		this->DamageType = DamageType;
		this->DamageCauser = DamageCauser;
		this->DamageInstigator = DamageInstigator;
	}
};

USTRUCT(BlueprintType)
struct FBodyCondition
{
	GENERATED_USTRUCT_BODY()

public:
	UPROPERTY(BlueprintReadOnly, Category = "General")
		float DamageTaken = 0.0f;
	UPROPERTY(BlueprintReadOnly, Category = "General")
		TArray<UDamageCondition*> Conditions;
};




UCLASS()
class FIRSTPERSONMODULE_API AFirstPersonCharacter : public ACharacter
{
	GENERATED_BODY()
public:


public:
	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = "Body Sockets")
		FName HeadCameraSocket = "HeadCameraSocket";

protected:
	/*inventory manager component*/
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Inventory")
		class UPlayerInventoryManagerComponent* InventoryManager;


	/*equipment slots*/
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Inventory")
		class UInventoryAttachmentComponent* Undershirt;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Inventory")
		class UInventoryAttachmentComponent* Underwear;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Inventory")
		class UInventoryAttachmentComponent* Pants;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Inventory")
		class UInventoryAttachmentComponent* Shirt;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Inventory")
		class UInventoryAttachmentComponent* Headwear;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Inventory")
		class UInventoryAttachmentComponent* Jacket;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Inventory")
		class UInventoryAttachmentComponent* Shoes;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Inventory")
		class UInventoryAttachmentComponent* Glasses;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Inventory")
		class UInventoryAttachmentComponent* Backpack;	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Inventory")
		class UInventoryAttachmentComponent* Vest;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Inventory")
		class UInventoryAttachmentComponent* Mask;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Inventory")
		class UInventoryAttachmentComponent* Gloves;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Inventory")
		class UInventoryAttachmentComponent* Watch;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Inventory")
		class UInventoryAttachmentComponent* Primary;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Inventory")
		class UInventoryAttachmentComponent* Secondary;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Inventory")
		class UInventoryAttachmentComponent* Alternative;








public:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Inventory")
		FName InventoryManagerComponentName = "InventoryManagerComponent";



	/*interactions manager component*/
protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Interactives")
		class UInteractiveManagerComponent* InteractiveManager;
public:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Interactives")
		FName InteractiveManagerComponentName = "InteractiveManagerComponent";

public:
	/*first person view component*/
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "First Person View")
		class UFirstPersonViewComponent* FirstPersonView;
public:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "First Person View")
		FName FirstPersonViewComponentName = "FirstPersonViewComponent";
protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Gore")
		class UGoreComponent* GoreComponent;
public:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Gore")
		FName GibComponentName = "GibComponent";



	/*health/damage*/
protected:
	UPROPERTY(BlueprintReadOnly,Replicated, Category = "Health")
		float Health = 100.0f;
	UPROPERTY()
		bool bDead = false;

	/*variable replicated to the client upon a character's death - this will send all the information need to replicate the death conditions of a character*/
	UPROPERTY(Replicated, ReplicatedUsing = "OnRep_ReceivedDeathInfo")
		FDeathInfo ReplicatedDeath;

public:	
	/*body parts*/
	UPROPERTY(BlueprintReadOnly, Category = "Heath|Body Damage")
		FBodyCondition Head;	
	UPROPERTY(BlueprintReadOnly, Category = "Heath|Body Damage")
		FBodyCondition RightEye;
	UPROPERTY(BlueprintReadOnly, Category = "Heath|Body Damage")
		FBodyCondition LeftEye;
	UPROPERTY(BlueprintReadOnly, Category = "Heath|Body Damage")
		FBodyCondition Chest;
	UPROPERTY(BlueprintReadOnly, Category = "Heath|Body Damage")
		FBodyCondition Back;
	UPROPERTY(BlueprintReadOnly, Category = "Heath|Body Damage")
		FBodyCondition RightArm;
	UPROPERTY(BlueprintReadOnly, Category = "Heath|Body Damage")
		FBodyCondition LeftArm;
	UPROPERTY(BlueprintReadOnly, Category = "Heath|Body Damage")
		FBodyCondition RightHand;
	UPROPERTY(BlueprintReadOnly, Category = "Heath|Body Damage")
		FBodyCondition LeftHand;
	UPROPERTY(BlueprintReadOnly, Category = "Heath|Body Damage")
		FBodyCondition RightLeg;
	UPROPERTY(BlueprintReadOnly, Category = "Heath|Body Damage")
		FBodyCondition LeftLeg;
	UPROPERTY(BlueprintReadOnly, Category = "Heath|Body Damage")
		FBodyCondition RightFoot;
	UPROPERTY(BlueprintReadOnly, Category = "Heath|Body Damage")
		FBodyCondition LeftFoot;

	/*additional parts*/
	UPROPERTY(BlueprintReadOnly, Category = "Heath|Body Damage")
		FBodyCondition Vision;

	UPROPERTY(BlueprintReadOnly, Category = "Health")
		float Stamina = 100.0f;	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Health")
		float MaxStamina = 100.0f;
	



	UPROPERTY(BlueprintReadOnly, Category = "Health")
		float Encumbrance = 0.0f;	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Health")
		float MaxEncumbrance = 180.0f;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Health")
		float BloodLevel = 5000;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Health")
		float Hydration = 5000;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Health")
		float Energy = 7500.0f;

	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Category = "Vitals")
		class UVitalsComponent* VitalsComponent;
	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Category = "Vitals")
		FName VitalsComponentName = "Vitals Component";

protected:
	/*input*/
	UPROPERTY(EditDefaultsOnly, Category = "Input")
		class UInputMappingContext* PlayerInputMapping;
	UPROPERTY(EditDefaultsOnly, Category = "Input|Actions|Movement")
		class UInputAction* ForwardMovementAction;
	UPROPERTY(EditDefaultsOnly, Category = "Input|Actions|Movement")
		class UInputAction* SidewaysMovementAction;

	UPROPERTY(EditDefaultsOnly, Category = "Input|Actions|Movement")
		class UInputAction* ToggleProneAction;
	UPROPERTY(EditDefaultsOnly, Category = "Input|Actions|Movement")
		class UInputAction* ToggleCrouchAction;

	UPROPERTY(EditDefaultsOnly, Category = "Input|Actions|Movement")
		class UInputAction* BeginSprintAction;
	UPROPERTY(EditDefaultsOnly, Category = "Input|Actions|Movement")
		class UInputAction* EndSprintAction;

	UPROPERTY(EditDefaultsOnly, Category = "Input|Actions|View")
		class UInputAction* PitchViewAction;
	UPROPERTY(EditDefaultsOnly, Category = "Input|Actions|View")
		class UInputAction* RotateViewAction;
	UPROPERTY(EditDefaultsOnly, Category = "Input|Actions|View")
		class UInputAction* ZoomVisionInAction;
	UPROPERTY(EditDefaultsOnly, Category = "Input|Actions|View")
		class UInputAction* ZoomVisionOutAction;


	UPROPERTY(EditDefaultsOnly, Category = "Input|Actions|Weapons")
		class UInputAction* BeginFireAction;
	UPROPERTY(EditDefaultsOnly, Category = "Input|Actions|Weapons")
		class UInputAction* EndFireAction;

	UPROPERTY(EditDefaultsonly, Category = "Input|Actions|Interactives")
		class UInputAction* BeginInteractionAction;
	UPROPERTY(EditDefaultsonly, Category = "Input|Actions|Interactives")
		class UInputAction* EndInteractionAction;

	UPROPERTY(EditDefaultsOnly, Category = "Input|Actions|Inventory")
		class UInputAction* ToggleInventoryAction;

public:
	UPROPERTY(VisibleAnywhere, Category = "Third Person Mesh")
		FName RightHandSocketName = "RightHandSocket";
	UPROPERTY(VisibleAnywhere, Category = "Third Person Mesh")
		FName LeftHandSocketName = "LeftHandSocket";


	//=========================================================================================================================================================
	//========================================================================FUNCTIONS========================================================================
	//=========================================================================================================================================================
public:
	// Sets default values for this character's properties
	AFirstPersonCharacter(const FObjectInitializer& ObjectInitializer);

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	//========================
	//======FIRST PERSON======
	//========================
public:
	UFUNCTION()
		class UFirstPersonViewComponent* GetFirstPersonView();

	//======================
	//========HEALTH========
	//======================
	UFUNCTION(BlueprintPure, Category = "Health & Stats")
		const float GetHealth() const;
	UFUNCTION(BlueprintPure, Category = "Health & Stats")
		const float GetBloodLevel() const;
	UFUNCTION()
		virtual void OnReceiveDamage(AActor* DamagedActor, float Damage, const class UDamageType* DamageType, class AController* InstigatedBy, AActor* DamageCauser);
	UFUNCTION()
		virtual void OnReceivePointDamage(AActor* DamagedActor, float Damage, class AController* InstigatedBy, FVector HitLocation, class UPrimitiveComponent* FHitComponent, FName BoneName, FVector ShotFromDirection, const class UDamageType* DamageType, AActor* DamageCauser);
	UFUNCTION()
		virtual void OnReceiveRadialDamage(AActor* DamagedActor, float Damage, const class UDamageType* DamageType, FVector Origin, const FHitResult& HitInfo, class AController* InstigatedBy, AActor* DamageCauser);
	
	UFUNCTION(BlueprintPure, Category = "Health & Stats")
		class UVitalsComponent* GetVitalsComponent();

	//=======================
	//========STAMINA========
	//=======================
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Player Stats|Stamina")
		const float GetStamina() const;
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Player Stats|Stamina")
		const float GetMaxStamina() const;
	
	//======================
	//========ENERGY========
	//======================
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Player Stats|Energy")
		const float GetEnergyLevel() const;
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Player Stats|Energy")
		const float GetHydrationLevel() const;

	//=========================
	//=======TEMPERATURE=======
	//=========================
public:
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Player Stats|Temperature")
		const float GetTemperature() const;

	//===========================
	//========GIBS & GORE========
	//===========================
	public:
		//UFUNCTION()
			//virtual void GibCharacter();

	//=======================
	//=========DEATH=========
	//=======================

public:
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Health")
		bool IsPlayerDead();

protected:
	/*this function will actually kill the player if they're not already dead, do some internal stuff, then call OnDeath for animations and visuals*/
	UFUNCTION(BlueprintCallable, Category = "Health")
		void Kill(const class UDamageType* DamageType, AActor* DamageCauser, AController* DamageInstigator);

	/*called by Killed() from the Server or from OnRep_ReceiveDeathInfo() for clients
	which handles the animations and visual side of things - dont' call directly*/
	UFUNCTION()
		virtual void OnDeath(const FDeathInfo DeathInfo, AActor* DamageCauser, AController* DamageInstigator);
protected:
	/*blueprint hook - called from OnDeath() at the end of its chain*/
	UFUNCTION(BlueprintImplementableEvent, Category = "Health", meta=(DisplayName = "On Death"))
		void BP_OnDeath(const FDeathInfo DeathInfo, AActor* DamageCauser, AController* DamageInstigator);

	/*called whenever the DeathInfo variable is received - allowing us to replicate a character's death on the client end*/
	UFUNCTION()
		virtual void OnRep_ReceivedDeathInfo();

	UFUNCTION(BlueprintCallable, Category = "Health")
		void Ragdoll();

	UFUNCTION()
		void EnableCapsuleCollision();
	UFUNCTION()
		void DisableCapsuleCollision();



	//=========================
	//========INVENTORY========
	//=========================
public:
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Inventory")
		UPlayerInventoryManagerComponent* GetInventoryManager() { return InventoryManager; };
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Inventory")
		virtual bool HasItemEquipped();
	UFUNCTION(BlueprintCallable, Category = "Inventory")
		void ToggleInventory();
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Inventory")
		float GetEncumbrance();
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Inventory")
		float GetMaxEncumbrance();

	//===========================
	//=========EQUIPMENT=========
	//===========================
public:
	UFUNCTION()
		virtual void OnItemEquipped(class AInventoryItem* Item, class UInventoryAttachmentComponent* EquippedComponent);
	/*returns the first equipment component for the desired itemtype*/
	UFUNCTION(BlueprintCallable, Category = "Inventory|Equipment")
		virtual class UInventoryAttachmentComponent* GetEquipmentSlot(TSubclassOf<AInventoryItem> ItemType);
	UFUNCTION(BlueprintCallable, Category = "Inventory|Equipment")
		virtual class UInventoryAttachmentComponent* GetEquipmentSlotByID(FName EquipmentID);

public:
	UFUNCTION()
		void OnInventoryManagerOpened();
	UFUNCTION()
		void OnInventoryManagerClosed();

	//============================
	//========INTERACTIVES========
	//============================
public:
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Interactions")
		UInteractiveManagerComponent* GetInteractionsManager() { return InteractiveManager; }
protected:
	UFUNCTION()
		virtual void BeginInteraction();
	UFUNCTION()
		virtual void EndInteraction();



	//=========================
	//========WEAPONS========
	//=========================
public:
	UFUNCTION()
		virtual void BeginFire();
	UFUNCTION()
		virtual void EndFire();
	UFUNCTION()
		virtual bool HasWeaponEquipped();

	//==========================
	//========ANIMATIONS========
	//==========================
public:
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Third Person")
		class UThirdPersonAnimInstance* GetThirdPersonAnimBlueprint();


	//========================
	//========MOVEMENT========
	//========================
private:
	UFUNCTION()
		virtual void MoveForward(const struct FInputActionValue& Value);
	UFUNCTION()
		virtual void MoveRight(const struct FInputActionValue& Value);
	UFUNCTION()
		virtual void ToggleCrouch();
	UFUNCTION()
		virtual void ToggleProne();
	UFUNCTION()
		virtual bool CanMove();

	UFUNCTION()
		virtual void BeginSprint();
	UFUNCTION()
		virtual void EndSprint();

	//========================
	//==========VIEW==========
	//========================
		
	UFUNCTION()
		virtual void PitchView(const struct FInputActionValue& Value);
	UFUNCTION()
		virtual void RotateView(const struct FInputActionValue& Value);
	UFUNCTION()
		virtual void ZoomVisionIn();
	UFUNCTION()
		virtual void ZoomVisionOut();
	UFUNCTION()
		virtual bool CanMoveCamera();

	//==========================
	//========CONTROLLER========
	//==========================
	UFUNCTION()
		class AFirstPersonPlayerController* GetFirstPersonController();
};
