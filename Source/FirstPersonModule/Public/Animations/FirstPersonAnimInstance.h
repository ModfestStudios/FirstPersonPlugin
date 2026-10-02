// Copyrighted : Modfest Studios 2025-2026

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "FirstPersonAnimInstance.generated.h"

/**
 * 
 */
UCLASS(abstract)
class FIRSTPERSONMODULE_API UFirstPersonAnimInstance : public UAnimInstance
{
	GENERATED_BODY()
public:


	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Movement")
		float MovementSpeed = 0.0f;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Movement")
		FVector2D MovementDirection = FVector2D::ZeroVector;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Movement")
			bool bIsMoving = false;




	/*character caching*/
	UPROPERTY()
		AFirstPersonCharacter* Character;
	UPROPERTY()
		UFirstPersonMovementComponent* MovementComponent;


	//=======================================================================================================================================================================================
	//=======================================================================================FUNCTIONS=======================================================================================
	//=======================================================================================================================================================================================


	virtual void NativeInitializeAnimation() override;
	virtual void NativeUpdateAnimation(float DeltaTimeX) override;
	virtual void NativeThreadSafeUpdateAnimation(float DeltaSeconds) override;

	UFUNCTION()
		virtual void UpdateMovement();


	//=======================================
	//===============UTILITIES===============
	//=======================================
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Character")
		class AFirstPersonCharacter* GetFirstPersonCharacter() const;
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Movement")
		class UFirstPersonMovementComponent* GetFirstPersonMovement() const;
	UFUNCTION(BlueprintCallable, Category = "Initialization")
		virtual void InitializeFirstPersonCharacterReferences();
	
};
