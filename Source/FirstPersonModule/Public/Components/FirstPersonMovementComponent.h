// Copyright (c) 2022 Pocket Sized Animations

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/CharacterMovementReplication.h"
#include "FirstPersonMovementComponent.generated.h"


UENUM(BlueprintType)
enum ECustomMovement : uint8
{
	CUSTMOVE_None				UMETA(Hidden),
	CUSTMOVE_Sliding			UMETA(DisplayName = "Sliding"),
	CUSTMOVE_Prone				UMETA(DisplayName = "Prone"),
	CUSTMOVE_Ladder				UMETA(DisplayName = "Ladder"),
	CUSTMOVE_WallRunning		UMETA(DisplayName = "Wall Running"),
	CUSTMOVE_Mantling			UMETA(DisplayName = "Mantling")
};

/**
 *
 */
UCLASS()
class FIRSTPERSONMODULE_API UFirstPersonMovementComponent : public UCharacterMovementComponent
{
	GENERATED_BODY()
public:

	class FMultiplayerNetworkMoveData : public FCharacterNetworkMoveData
	{
	public:
		typedef FCharacterNetworkMoveData Super;

		virtual void ClientFillNetworkMoveData(const FSavedMove_Character& ClientMove, ENetworkMoveType MoveType) override;
		virtual bool Serialize(UCharacterMovementComponent& CharacterMovement, FArchive& Ar, UPackageMap* PackageMap, ENetworkMoveType MoveType) override;

		uint8 MoveData_CustomMovementFlags = 0;
	};

	class FMultiplayerNetworkMoveDataContainer : public FCharacterNetworkMoveDataContainer
	{
	public:
		FMultiplayerNetworkMoveDataContainer();

		FMultiplayerNetworkMoveData CustomDefaultMoveData[3];
	};

	//============================================================================================================================
	//=========================================================SAVED MOVE=========================================================
	//============================================================================================================================
	class FMultiplayerSavedMove : public FSavedMove_Character
	{
	public:
		typedef FSavedMove_Character Super;

		/*variables we're using to snapshot movement*/
		uint8 Saved_bWantsToLeanLeft : 1;
		uint8 Saved_bWantsToLeanRight : 1;
		uint8 Saved_bWantsToSprint : 1;

		uint8 Saved_CustomMovementFlags;

		uint8 Saved_bWantsToProne : 1;
		uint8 Saved_bPrevWantsToCrouch : 1;

		//===============================================================================================
		//===========================================FUNCTIONS===========================================
		//===============================================================================================
		// 
		//Resetse all saved variables
		virtual void Clear() override;
		//Stores input commands in compressed flags (reducing network packet size)
		virtual uint8 GetCompressedFlags() const override;
		//checks if moves can be combined for efficiency w/ network bandwidth
		virtual bool CanCombineWith(const FSavedMovePtr& NewMove, ACharacter* InCharacter, float MaxDelta) const override;
		//Configures the move for sending to server
		virtual void SetMoveFor(ACharacter* C, float InDeltaTime, FVector const& NewAccel, FNetworkPredictionData_Client_Character& ClientData) override;
		//Configurees variables on Character Move Component making a correction
		virtual void PrepMoveFor(ACharacter* C) override;


	};

	//=========================================================================================================================
	//=====================================================PREDICTION DATA=====================================================
	//=========================================================================================================================
	class FMultiplayerNetworkPredictionData_Client : public FNetworkPredictionData_Client_Character
	{
	public:
		FMultiplayerNetworkPredictionData_Client(const UCharacterMovementComponent& ClientMovement);

		typedef FNetworkPredictionData_Client_Character Super;

		virtual FSavedMovePtr AllocateNewMove() override;
	};

	friend class FMultipalyerSavedMove;


	//==========================================================================================================================================================
	//===============================================================CHARACTER MOVEMENT COMPONENT===============================================================
	//==========================================================================================================================================================



	//==================================================================
	//============================PROPERTIES============================
	//==================================================================
public:

	FMultiplayerNetworkMoveDataContainer CustomMoveDataContainer;

	enum ECustomMovementFlags
	{
		CFLAG_Sprint = 1 << 0,
		CFLAG_TactiWalk = 1 << 1
	};

	uint8 CustomMovementFlags = 0;
	bool bSprinting = false; //CFLAG_Custom1

	bool bLeaningLeft = false; //CFLAG_RESERVED_1
	bool bLeaningRight = false; //CFLAG_RESERVED_2

	/*reference to our owning character*/
	UPROPERTY(Transient)
		class AFirstPersonCharacter* FirstPersonOwner;


	/*sprinting*/
	UPROPERTY(EditAnywhere, Category = "Sprinting")
		float MaxSprintSpeed = 1200;

	/*sliding*/
	bool bPrevWantsToCrouch;
	UPROPERTY(EditDefaultsOnly, Category = "Sliding")
		float MaxSlideSpeed = 700.0f;
	UPROPERTY(EditDefaultsOnly, Category = "Sliding")
		float MinSlideSpeed = 350.0f;
	UPROPERTY(EditDefaultsOnly, Category = "Sliding")
		float SlideImpulseSpeed = 500;
	UPROPERTY(EditDefaultsOnly, Category = "Sliding")
		float SlideGravityForce = 5000;
	UPROPERTY(EditDefaultsOnly, Category = "Sliding")
		float SlideFriction = 1.3f;
	UPROPERTY(EditDefaultsOnly, Category = "Sliding")
		float SlideBrakingDeceleration = 1000.f;

	/*prone*/
	UPROPERTY()
		bool bWantsToProne = false;
	UPROPERTY(EditDefaultsOnly, Category = "Prone")
		float ProneEnterHoldDuration = .2f;
	UPROPERTY(EditDefaultsOnly, Category = "Prone")
		float ProneSlideEnterImpulse = 180.0f;
	UPROPERTY(EditDefaultsOnly, Category = "Prone")
		float ProneMaxSpeed = 200.f;
	UPROPERTY(EditDefaultsOnly, Category = "Prone")
		float ProneBrakingDeceleration = 2500.0f;
	UPROPERTY(EditDefaultsOnly, Category = "Prone")
		float ProneHalfHeight = 25.0f;


	//=====================================================================================================================
	//======================================================FUNCTIONS======================================================
	//=====================================================================================================================
public:
	UFirstPersonMovementComponent();

protected:
	virtual void InitializeComponent() override;

public:
	virtual void ActivateCustomMovementFlag(ECustomMovementFlags Flag);
	virtual void ClearMovementFlag(ECustomMovementFlags Flag);
	bool IsCustomFlagSet(ECustomMovementFlags Flag) const { return (CustomMovementFlags & Flag) != 0; }

	virtual void UpdateFromCompressedFlags(uint8 Flags) override;
	virtual class FNetworkPredictionData_Client* GetPredictionData_Client() const override;
	virtual void MoveAutonomous(float ClientTimeStamp, float DeltaTime, uint8 CompressedFlags, const FVector& NewAccel) override;

	UFUNCTION()
		virtual bool IsCustomMovementMode(ECustomMovement IsCustomMovementMode) const;
	virtual void OnMovementModeChanged(EMovementMode PreviousMovementMode, uint8 PreviousCustomMode) override;

	//======================================================================================================
	//===============================================MOVEMENT===============================================
	//======================================================================================================


	virtual void UpdateCharacterStateBeforeMovement(float DeltaSeconds) override;
	virtual void OnMovementUpdated(float deltaSeconds, const FVector& OldLocation, const FVector& OldVelocity) override;
	virtual bool IsMovingOnGround() const override;
	virtual bool CanCrouchInCurrentState() const override;
	virtual float GetMaxSpeed() const override;

	virtual void PhysCustom(float deltaTime, int32 Iterations) override;

	//=========================================================
	//=========================SLIDING=========================
	//=========================================================
public:
	UFUNCTION()
		virtual void EnterSlide(EMovementMode PrevMode, ECustomMovement PrevCustomMode);
	UFUNCTION()
		virtual void ExitSlide();
protected:
	UFUNCTION()
		virtual void PhysSlide(float deltaTime, int32 Iterations);
	UFUNCTION()
		virtual bool GetSlideSurface(FHitResult& Hit) const;

	//=========================================================
	//========================CROUCHING========================
	//=========================================================
public:
	UFUNCTION(BlueprintCallable, Category = "Crouching")
		virtual void ToggleCrouch();
	UFUNCTION(BlueprintCallable, Category = "Crouching")
		virtual void BeginCrouch();
	UFUNCTION(BlueprintCallable, Category = "Crouching")
		virtual void EndCrouch();


	//=======================================================
	//=========================PRONE=========================
	//=======================================================
public:
	UFUNCTION(BlueprintCallable, Category = "Prone")
		virtual void ToggleProne();
	UFUNCTION(BlueprintCallable, Category = "Prone")
		virtual void BeginProne();
	UFUNCTION(BlueprintCallable, Category = "Prone")
		virtual void EndProne();
	UFUNCTION()
		bool CanGoProne() const;

protected:
	UFUNCTION()
		virtual void EnterProne(EMovementMode PrevMode, ECustomMovement PrevCustomMode);
	UFUNCTION()
		virtual void ExitProne();

	UFUNCTION()
		void PhysProne(float deltaTime, int32 Iterations);


	//=========================================================
	//========================SPRINTING========================
	//=========================================================
public:
	UFUNCTION(BlueprintCallable, Category = "Sprinting")
		virtual void BeginSprint();
	UFUNCTION(BlueprintCallable, Category = "Sprinting")
		virtual void EndSprint();

protected:
	/*obsolete....? We use Unreal's flags not custom flag anymore*/
	UFUNCTION(BlueprintCallable, Category = "Multiplayer Movement")
		void ToggleSprintingFlag(bool bShouldSprint);


	//=========================================================
	//=========================LEANING=========================
	//=========================================================
public:
	UFUNCTION()
		virtual void LeanLeft();
	UFUNCTION()
		virtual void LeanRight();

	//===========================================================
	//==========================CAPSULE==========================
	//===========================================================
protected:
	UFUNCTION()
		virtual void SetCapsuleHeight(float NewHeight);
};


