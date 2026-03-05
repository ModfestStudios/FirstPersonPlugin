// Copyrighted Modfest Studios 2024


#include "Characters/FirstPersonCharacter.h"
#include "Players/FirstPersonPlayerController.h"

/*animations*/
#include "Animations/FirstPersonArmsAnimInstance.h"
#include "Animations/ThirdPersonAnimInstance.h"
#include "Animation/AnimInstance.h"


/*curves*/
#include "Curves/CurveFloat.h"

/*components*/
#include "Components/SkeletalMeshComponent.h"
#include "Components/CapsuleComponent.h"

/*engine*/
#include "Engine/GameInstance.h"
#include "Engine/World.h"


/*framwork*/
#include "GameFramework/DamageType.h"
#include "Engine/SkeletalMesh.h"

/*gore*/
#include "Components/GoreComponent.h"
#include "Subsystems/GoreSubsystem.h"

/*inventory*/
#include "Components/InventoryManagerComponent.h"
#include "Components/InventoryItemComponent.h"
#include "Weapons/Weapon.h"
#include "Weapons/Firearm.h"

/*interactives*/
#include "Components/InteractiveManagerComponent.h"

/*movement*/
#include "Components/FirstPersonMovementComponent.h"

/*first person*/
#include "Components/FirstPersonViewComponent.h"
#include "Animations/FirstPersonArmsAnimInstance.h"

/*input*/
#include "InputAction.h"
#include "InputMappingContext.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"

/*materials*/
#include "Materials/Material.h"
#include "Materials/MaterialInterface.h"
#include "Materials/MaterialInstance.h"
#include "Materials/MaterialInstanceDynamic.h"

/*subsystems*/
#include "Subsystems/GameMasterSubsystem.h"
#include "Subsystems/CharacterSubsystem.h"

/*network*/
#include "Net/UnrealNetwork.h"

/*ui*/
#include "Blueprint/UserWidget.h"
#include "UI/Widgets/PlayerHUDWidget.h"

/*utilities*/
#include "UObject/ConstructorHelpers.h"



void AFirstPersonCharacter::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AFirstPersonCharacter, Health);
	DOREPLIFETIME(AFirstPersonCharacter, ReplicatedDeath);		
}

// Sets default values
AFirstPersonCharacter::AFirstPersonCharacter(const FObjectInitializer& ObjectInitializer) :
	Super(ObjectInitializer.SetDefaultSubobjectClass<UFirstPersonMovementComponent>(CharacterMovementComponentName))
{
	FirstPersonView = ObjectInitializer.CreateDefaultSubobject<UFirstPersonViewComponent>(this, FirstPersonViewComponentName);
	if (FirstPersonView)
	{
		static ConstructorHelpers::FObjectFinder<USkeletalMesh> armMeshAsset(TEXT("/Script/Engine.SkeletalMesh'/Game/Characters/_Base/Mesh/FirstPersonArms_Male_Ideal.FirstPersonArms_Male_Ideal'"));
		if (armMeshAsset.Succeeded())
			FirstPersonView->SetFirstPersonArms(armMeshAsset.Object);

		FirstPersonView->bAllowZoom = true;
		FirstPersonView->bAllowFreeLook = true;
		FirstPersonView->bAutoCalcVerticalFreeLook = true;	

		static ConstructorHelpers::FClassFinder<UPlayerHUDWidget> hudAsset(TEXT("/FirstPersonModule/UI/Blueprints/BP_DefaultPlayerHud"));
		if (hudAsset.Succeeded())
			FirstPersonView->HUDClass = hudAsset.Class;

		static ConstructorHelpers::FObjectFinder<UMaterial> ppAsset(TEXT("/FirstPersonModule/Materials/Overlays/PP_Overlay_RectangularVignette_SoftCenter_MasterMaterial"));
		static ConstructorHelpers::FObjectFinder<UCurveFloat> curveAsset(TEXT("/FirstPersonModule/Blueprints/Curves/RampUpDown"));
		if (ppAsset.Succeeded() && curveAsset.Succeeded())
		{			
			UMaterialInterface* Material = ppAsset.Object.Get();
			UCurveFloat* Curve = curveAsset.Object.Get();
			FirstPersonView->DamageIndicators.Add(FDamageIndicator(Material,1.0f, 1.0f, Curve));
		}
		
	}

	InventoryManager = ObjectInitializer.CreateDefaultSubobject<UInventoryManagerComponent>(this, InventoryManagerComponentName);
	if (InventoryManager)
	{

	}

	InteractiveManager = ObjectInitializer.CreateDefaultSubobject<UInteractiveManagerComponent>(this, InteractiveManagerComponentName);
	if (InteractiveManager)
	{
		
	}

	GoreComponent = ObjectInitializer.CreateDefaultSubobject<UGoreComponent>(this, GibComponentName);
	if (GoreComponent)
	{

	}

	if (GetMesh())
	{
		static ConstructorHelpers::FObjectFinder<USkeletalMesh> DefaultPlayerMeshRef(TEXT("/FirstPersonModule/Characters/Mesh/MaleMannequin_Quin"));
		if (DefaultPlayerMeshRef.Succeeded())
			GetMesh()->SkeletalMesh = DefaultPlayerMeshRef.Object;

		GetMesh()->SetRelativeLocation(FVector(0, 0, -90));
		GetMesh()->bOwnerNoSee = true;
	}

	/*default mapping inputs*/
	static ConstructorHelpers::FObjectFinder<UInputMappingContext> DefaultMappingInputContextRef(TEXT("/FirstPersonModule/Blueprints/Input/Mappings/DefaultPlayerMapping"));
	if (DefaultMappingInputContextRef.Succeeded())
		PlayerInputMapping = DefaultMappingInputContextRef.Object;
	//forward
	{
		static ConstructorHelpers::FObjectFinder<UInputAction> DefaultInputActionRef(TEXT("/FirstPersonModule/Blueprints/Input/Actions/ForwardMovement"));
		if (DefaultInputActionRef.Succeeded())
			ForwardMovementAction = DefaultInputActionRef.Object;
	}
	//sideways
	{
		static ConstructorHelpers::FObjectFinder<UInputAction> DefaultInputActionRef(TEXT("/FirstPersonModule/Blueprints/Input/Actions/SidewaysMovement"));
		if (DefaultInputActionRef.Succeeded())
			SidewaysMovementAction = DefaultInputActionRef.Object;
	}
	//prone
	{
		static ConstructorHelpers::FObjectFinder<UInputAction> DefaultInputActionRef(TEXT("/FirstPersonModule/Blueprints/Input/Actions/ToggleProne"));
		if (DefaultInputActionRef.Succeeded())
			ToggleProneAction = DefaultInputActionRef.Object;
	}
	//crouch
	{
		static ConstructorHelpers::FObjectFinder<UInputAction> DefaultInputActionRef(TEXT("/FirstPersonModule/Blueprints/Input/Actions/ToggleCrouch"));
		if (DefaultInputActionRef.Succeeded())
			ToggleCrouchAction = DefaultInputActionRef.Object;
	}
	//begin srpint
	{
		static ConstructorHelpers::FObjectFinder<UInputAction> DefaultInputActionRef(TEXT("/FirstPersonModule/Blueprints/Input/Actions/BeginSprint"));
		if (DefaultInputActionRef.Succeeded())
			BeginSprintAction = DefaultInputActionRef.Object;
	}
	//begin srpint
	{
		static ConstructorHelpers::FObjectFinder<UInputAction> DefaultInputActionRef(TEXT("/FirstPersonModule/Blueprints/Input/Actions/EndSprint"));
		if (DefaultInputActionRef.Succeeded())
			EndSprintAction = DefaultInputActionRef.Object;
	}
	//pitch view
	{
		static ConstructorHelpers::FObjectFinder<UInputAction> DefaultInputActionRef(TEXT("/FirstPersonModule/Blueprints/Input/Actions/PitchView"));
		if (DefaultInputActionRef.Succeeded())
			PitchViewAction = DefaultInputActionRef.Object;
	}
	//rotate view
	{
		static ConstructorHelpers::FObjectFinder<UInputAction> DefaultInputActionRef(TEXT("/FirstPersonModule/Blueprints/Input/Actions/RotateView"));
		if (DefaultInputActionRef.Succeeded())
			RotateViewAction = DefaultInputActionRef.Object;
	}
	//zoom in view
	{
		static ConstructorHelpers::FObjectFinder<UInputAction> DefaultInputActionRef(TEXT("/FirstPersonModule/Blueprints/Input/Actions/ZoomIn"));
		if (DefaultInputActionRef.Succeeded())
			ZoomVisionInAction = DefaultInputActionRef.Object;
	}
	//zoom out view
	{
		static ConstructorHelpers::FObjectFinder<UInputAction> DefaultInputActionRef(TEXT("/FirstPersonModule/Blueprints/Input/Actions/ZoomOut"));
		if (DefaultInputActionRef.Succeeded())
			ZoomVisionOutAction = DefaultInputActionRef.Object;
	}
	//fire weapon
	{
		static ConstructorHelpers::FObjectFinder<UInputAction> DefaultInputActionRef(TEXT("/FirstPersonModule/Blueprints/Input/Actions/FireWeapon"));
		if (DefaultInputActionRef.Succeeded())
			BeginFireAction = DefaultInputActionRef.Object;
	}
	//end fire
	{
		static ConstructorHelpers::FObjectFinder<UInputAction> DefaultInputActionRef(TEXT("/FirstPersonModule/Blueprints/Input/Actions/StopFiringWeapon"));
		if (DefaultInputActionRef.Succeeded())
			EndFireAction = DefaultInputActionRef.Object;
	}
	//begin interaction
	{
		static ConstructorHelpers::FObjectFinder<UInputAction> DefaultInputActionRef(TEXT("/FirstPersonModule/Blueprints/Input/Actions/BeginInteraction"));
		if (DefaultInputActionRef.Succeeded())
			BeginInteractionAction = DefaultInputActionRef.Object;
	}
	//end interaction
	{
		static ConstructorHelpers::FObjectFinder<UInputAction> DefaultInputActionRef(TEXT("/FirstPersonModule/Blueprints/Input/Actions/EndInteraction"));
		if (DefaultInputActionRef.Succeeded())
			EndInteractionAction = DefaultInputActionRef.Object;
	}
	//inventory
	{
		static ConstructorHelpers::FObjectFinder<UInputAction> DefaultInputActionRef(TEXT("/FirstPersonModule/Blueprints/Input/Actions/ToggleInventoryMenu"));
		if (DefaultInputActionRef.Succeeded())
			ToggleInventoryAction = DefaultInputActionRef.Object;
	}




	
	PrimaryActorTick.bCanEverTick = true;
}

// Called when the game starts or when spawned
void AFirstPersonCharacter::BeginPlay()
{
	Super::BeginPlay();

	/*bind damage to new functions*/
	OnTakeAnyDamage.AddUniqueDynamic(this, &AFirstPersonCharacter::OnReceiveDamage);
	OnTakePointDamage.AddUniqueDynamic(this, &AFirstPersonCharacter::OnReceivePointDamage);
	OnTakeRadialDamage.AddUniqueDynamic(this, &AFirstPersonCharacter::OnReceiveRadialDamage);
}

// Called every frame
void AFirstPersonCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

// Called to bind functionality to input
void AFirstPersonCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);


	APlayerController* playerController = Cast<APlayerController>(GetController());
	UEnhancedInputComponent* Input = Cast<UEnhancedInputComponent>(PlayerInputComponent);

	/*attach standard player movement context*/
	if (IsValid(playerController) && IsValid(PlayerInputMapping) && playerController->IsLocalPlayerController())
	{
		if (UEnhancedInputLocalPlayerSubsystem* inputSubsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(playerController->GetLocalPlayer()))
		{
			inputSubsystem->AddMappingContext(PlayerInputMapping, 0);
		}
	}

	/*bind actions up*/
	Input->BindAction(ForwardMovementAction, ETriggerEvent::Triggered, this, FName("MoveForward"));
	Input->BindAction(SidewaysMovementAction, ETriggerEvent::Triggered, this, FName("MoveRight"));
	/*crouch*/
	Input->BindAction(ToggleCrouchAction, ETriggerEvent::Triggered, this, FName("ToggleCrouch"));
	//Input->BindAction(CrouchedReleasedAction, ETriggerEvent::Triggered, this, FName("EndCrouch"));
	/*prone*/
	Input->BindAction(ToggleProneAction, ETriggerEvent::Triggered, this, FName("ToggleProne"));
	//Input->BindAction(EndProneAction, ETriggerEvent::Triggered, this, FName("EndProne"));
	/*sprint*/
	Input->BindAction(BeginSprintAction, ETriggerEvent::Triggered, this, FName("BeginSprint"));
	Input->BindAction(EndSprintAction, ETriggerEvent::Triggered, this, FName("EndSprint"));
	/*pitch/rotate*/
	Input->BindAction(PitchViewAction, ETriggerEvent::Triggered, this, FName("PitchView"));
	Input->BindAction(RotateViewAction, ETriggerEvent::Triggered, this, FName("RotateView"));
	/*camera zoom*/
	Input->BindAction(ZoomVisionInAction, ETriggerEvent::Triggered, this, FName("ZoomVisionIn"));
	Input->BindAction(ZoomVisionOutAction, ETriggerEvent::Triggered, this, FName("ZoomVisionOut"));
	/*weapon firing*/
	Input->BindAction(BeginFireAction, ETriggerEvent::Triggered, this, FName("BeginFire"));
	Input->BindAction(EndFireAction, ETriggerEvent::Triggered, this, FName("EndFire"));

	/*interactions*/
	Input->BindAction(BeginInteractionAction, ETriggerEvent::Triggered, this, FName("BeginInteraction"));
	Input->BindAction(EndInteractionAction, ETriggerEvent::Triggered, this, FName("EndInteraction"));

	/*inventory*/
	Input->BindAction(ToggleInventoryAction, ETriggerEvent::Triggered, this, FName("ToggleInventory"));
}

//========================
//========MOVEMENT========
//========================
void AFirstPersonCharacter::MoveForward(const FInputActionValue& Value)
{
	if (!CanMove())
		return;

	/*initialize*/
	float inputValue = Value.Get<float>();

	/*safety check*/
	if (inputValue == 0.0f || GetCharacterMovement() == NULL)
		return;

	FVector inputVector = GetActorForwardVector() * inputValue;
	GetCharacterMovement()->AddInputVector(inputVector);
}

void AFirstPersonCharacter::MoveRight(const FInputActionValue& Value)
{
	if (!CanMove())
		return;

	/*initialization*/
	float inputValue = Value.Get<float>();

	/*safety check*/
	if (inputValue == 0.0f || GetCharacterMovement() == nullptr)
		return;

	FVector inputVector = GetActorRightVector() * inputValue;
	GetCharacterMovement()->AddInputVector(inputVector);
}

void AFirstPersonCharacter::ToggleCrouch()
{
	GetCharacterMovement<UFirstPersonMovementComponent>()->ToggleCrouch();
}



void AFirstPersonCharacter::ToggleProne()
{
	GetCharacterMovement<UFirstPersonMovementComponent>()->ToggleProne();
}

bool AFirstPersonCharacter::CanMove()
{
	if (bDead)
		return false;


	return true;
}


void AFirstPersonCharacter::BeginSprint()
{
	//GetCharacterMovement<UFirstPersonMovementComponent>()->ToggleSprintingFlag(true);
	GetCharacterMovement<UFirstPersonMovementComponent>()->BeginSprint();
}

void AFirstPersonCharacter::EndSprint()
{
	//GetCharacterMovement<UFirstPersonMovementComponent>()->ToggleSprintingFlag(false);
	GetCharacterMovement<UFirstPersonMovementComponent>()->EndSprint();
}

void AFirstPersonCharacter::PitchView(const FInputActionValue& Value)
{
	if (!CanMoveCamera())
		return;

	/*initialize*/
	float inputValue = Value.Get<float>();

	/*sensitivity hack*/
	inputValue *= 1.0f;

	if (inputValue == 0.0f || GetFirstPersonView() == nullptr)
		return;

	GetFirstPersonView()->PitchView(inputValue);
}

void AFirstPersonCharacter::RotateView(const FInputActionValue& Value)
{
	if (!CanMoveCamera())
		return;


	/*initialize*/
	float inputValue = Value.Get<float>();
	/*sensitivity hack*/
	inputValue *= 1.0f;

	if (inputValue == 0.0f || GetFirstPersonView() == nullptr)
		return;

	GetFirstPersonView()->RotateView(inputValue);
}

void AFirstPersonCharacter::ZoomVisionIn()
{
	if (GetFirstPersonView())
		GetFirstPersonView()->ZoomInVision();
}

void AFirstPersonCharacter::ZoomVisionOut()
{
	if (GetFirstPersonView())
		GetFirstPersonView()->ZoomOutVision();
}

bool AFirstPersonCharacter::CanMoveCamera()
{
	if (bDead)
		return false;

	return true;
}

AFirstPersonPlayerController* AFirstPersonCharacter::GetFirstPersonController()
{
	return Cast<AFirstPersonPlayerController>(GetController());
}

//========================
//======FIRST PERSON======
//========================
UFirstPersonViewComponent* AFirstPersonCharacter::GetFirstPersonView()
{
	return FirstPersonView;
}

//============================
//===========DAMAGE===========
//============================

const float AFirstPersonCharacter::GetHealth() const
{
	return Health;
}

const float AFirstPersonCharacter::GetBloodLevel() const
{
	return BloodLevel;
}

void AFirstPersonCharacter::OnReceiveDamage(AActor* DamagedActor, float Damage, const UDamageType* DamageType, AController* InstigatedBy, AActor* DamageCauser)
{
	/*we don't want to do anything against non-damage*/
	if (Damage <= 0.0f)
		return;
	
	/*flash damage indicators*/
	if (FirstPersonView && DamageType)
	{
		FirstPersonView->FlashDamageIndicator(DamageType->GetClass());
	}
	
	Health -= Damage;



	if (Health <= 0.0f && !IsPlayerDead())
		Kill(DamageType,DamageCauser,InstigatedBy);


	//UE_LOG(LogTemp, Log, TEXT("%s::OnReceiveDamage()"), *GetName());
	//FString DamageText = FString::Printf(TEXT("OnReceiveDamage(% f)"), Damage);
	//GEngine->AddOnScreenDebugMessage(626, 1.0f, FColor::Emerald, DamageText);
}

void AFirstPersonCharacter::OnReceivePointDamage(AActor* DamagedActor, float Damage, AController* InstigatedBy, FVector HitLocation, UPrimitiveComponent* FHitComponent, FName BoneName, FVector ShotFromDirection, const UDamageType* DamageType, AActor* DamageCauser)
{

	FString DamageText = FString::Printf(TEXT("OnReceivePOINTDamage(%f,%s,%s,%s,%s)"), Damage, *GetNameSafe(InstigatedBy), *HitLocation.ToString(), *GetNameSafe(FHitComponent), *BoneName.ToString());
	//UE_LOG(LogTemp, Log, TEXT("%s::OnReceivePointDamage(DAMAGE: %f, INSTIGATOR: %s, HITLOCATION: %s, COMPONENT: %s, BONE: %s)"), *GetName(), Damage, *GetNameSafe(InstigatedBy), *HitLocation.ToString(), *GetNameSafe(FHitComponent), *BoneName.ToString());
	//GEngine->AddOnScreenDebugMessage(627, 1.0f, FColor::Emerald, DamageText);

}

void AFirstPersonCharacter::OnReceiveRadialDamage(AActor* DamagedActor, float Damage, const UDamageType* DamageType, FVector Origin, const FHitResult& HitInfo, AController* InstigatedBy, AActor* DamageCauser)
{
	//UE_LOG(LogTemp, Log, TEXT("%s::OnReceiveRadialDamage()"), *GetName());
	//FString DamageText = FString::Printf(TEXT("OnReceiveRADIALdamage(% f)"), Damage);
	//GEngine->AddOnScreenDebugMessage(628, 1.0f, FColor::Emerald, DamageText);
}

const float AFirstPersonCharacter::GetStamina() const
{
	return Stamina;
}

const float AFirstPersonCharacter::GetMaxStamina() const
{
	return MaxStamina;
}

const float AFirstPersonCharacter::GetEnergyLevel() const
{
	return Energy;
}

const float AFirstPersonCharacter::GetHydrationLevel() const
{
	return Hydration;
}

const float AFirstPersonCharacter::GetTemperature() const
{
	return Temperature;
}


bool AFirstPersonCharacter::IsPlayerDead()
{
	return bDead;
}

void AFirstPersonCharacter::Kill(const UDamageType* DamageType, AActor* DamageCauser, AController* DamageInstigator)
{
	/*flag as dead*/
	bDead = true;

	/*generate the death info that gets sent to clients that can then be used to replicate the death on their end*/
	UDamageType* Damage = const_cast<UDamageType*>(DamageType);
	ReplicatedDeath = FDeathInfo(Damage,DamageInstigator,DamageInstigator); //this is a replicated variable that triggers Client logic
		 

	/*call event to handle death animations and ragdolling*/
	OnDeath(ReplicatedDeath, DamageCauser, DamageInstigator);
}

/* OnDeath() - Called by the Server Directly, or on a Client upon receiving the replicated variable DeathInfo (OnRep_OnReceiveDeathInfo)
* this is the safe version of "Kill" where we can modify what happens to a character upon death
*
*
*/
void AFirstPersonCharacter::OnDeath(const FDeathInfo DeathInfo, AActor* DamageCauser, AController* DamageInstigator)
{	
	
	/*prevent player from moving any further*/
	DisableInput(GetController<AFirstPersonPlayerController>());
	
	if (GetFirstPersonView())
	{
		/*attach camera to head to follow animation*/
		GetFirstPersonView()->AttachCameraToHead();
		/*hide first person arms*/
		GetFirstPersonView()->GetArmsMeshComponent()->SetVisibility(false);
	}

	/*enable visiblity of mesh*/
	if (GetMesh())
	{
		GetMesh()->SetOwnerNoSee(false);
	}

	/*notify 3rd person animation instance*/
	if (UThirdPersonAnimInstance* AnimInstance = GetThirdPersonAnimBlueprint())
		AnimInstance->OnDeath(DeathInfo);
	
	/*if this was a player-controlled pawn - notify Character so it can provide an event*/
	if (IsPlayerControlled())
		GetGameInstance()->GetSubsystem<UCharacterSubsystem>()->OnNotifiedOfPlayerDeath(GetFirstPersonController(), this, DamageCauser, DamageInstigator,DeathInfo.DamageType);

	//if (GetFirstPersonController() && GetNetMode() < NM_Client)
		//GetFirstPersonController()->EnterSpectatorMode();

	/*call the local blueprint event*/
	BP_OnDeath(DeathInfo, DamageCauser,DamageInstigator);
}

/*OnRep_ReceivedDeathInfo() - This function is called whenever a Client receives Networked Death information in order to simulate how this character died*/
void AFirstPersonCharacter::OnRep_ReceivedDeathInfo()
{	
	/*flag that this character is dead*/
	bDead = true;

	/*take the replicated variable and pass it to our OnDeath() function to be simulated on the client's end*/
	OnDeath(ReplicatedDeath,ReplicatedDeath.DamageCauser,ReplicatedDeath.DamageInstigator);
}

void AFirstPersonCharacter::Ragdoll()
{
	/*bails out of the function if no character mesh exists - to avoid confusion*/
	if (!GetMesh())
		return;
	
	if (GetMesh())
	{
		GetMesh()->SetCollisionProfileName(TEXT("Ragdoll"));
		GetMesh()->SetCollisionEnabled(ECollisionEnabled::PhysicsOnly);		
		GetMesh()->SetSimulatePhysics(true);
		GetMesh()->SetAllBodiesSimulatePhysics(true);
		GetMesh()->WakeAllRigidBodies();
	}

	if (GetCharacterMovement())
	{
		GetCharacterMovement()->StopMovementImmediately();
		GetCharacterMovement()->DisableMovement();
		GetCharacterMovement()->SetComponentTickEnabled(false);
	}

	DisableCapsuleCollision();

}

void AFirstPersonCharacter::EnableCapsuleCollision()
{
	if (GetCapsuleComponent())
	{
		GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	}
}

void AFirstPersonCharacter::DisableCapsuleCollision()
{
	if (GetCapsuleComponent())
	{
		GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	}
}

//=========================
//========INVENTORY========
//=========================

bool AFirstPersonCharacter::HasItemEquipped()
{
	if (InventoryManager)
	{
		if (IsValid(InventoryManager->GetCurrentlyEquippedItem()))
			return true;
	}

	return false;
}

void AFirstPersonCharacter::ToggleInventory()
{
	if (InventoryManager)
	{
		if (InventoryManager->IsInventoryUIOpen())
			InventoryManager->CloseInventoryUI();
		else
			InventoryManager->OpenInventoryUI();
	}
}

float AFirstPersonCharacter::GetEncumbrance()
{
	return Encumbrance;
}

float AFirstPersonCharacter::GetMaxEncumbrance()
{
	return MaxEncumbrance;
}

//============================
//========INTERACTIONS========
//============================


void AFirstPersonCharacter::BeginInteraction()
{
	if (GetInteractionsManager())
	{
		GetInteractionsManager()->Interact();
	}
}

void AFirstPersonCharacter::EndInteraction()
{
	if (GetInteractionsManager())
	{
		GetInteractionsManager()->StopInteraction();
	}
}

void AFirstPersonCharacter::BeginFire()
{
	if (InventoryManager)
	{
		if (AFirearm* Firearm = Cast<AFirearm>(InventoryManager->GetCurrentlyEquippedItem()))
			Firearm->BeginFire();
	}
}

void AFirstPersonCharacter::EndFire()
{
	if (InventoryManager)
	{
		if (AFirearm* Firearm = Cast<AFirearm>(InventoryManager->GetCurrentlyEquippedItem()))
			Firearm->EndFire();
	}
}

bool AFirstPersonCharacter::HasWeaponEquipped()
{
	if (InventoryManager)
	{
		if (AActor* InHand = InventoryManager->GetCurrentlyEquippedItem())
		{
			if (InHand->IsA<AWeapon>())
				return true;
		}
	}

	return false;
}

UThirdPersonAnimInstance* AFirstPersonCharacter::GetThirdPersonAnimBlueprint()
{
	if (GetMesh() == nullptr)
		return nullptr;

	return Cast<UThirdPersonAnimInstance>(GetMesh()->GetAnimInstance());	
}
