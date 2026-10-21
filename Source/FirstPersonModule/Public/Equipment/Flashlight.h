// Copyrighted : Modfest Studios 2025-2026

#pragma once

#include "CoreMinimal.h"
#include "Equipment/Equipment.h"
#include "Flashlight.generated.h"

/**
 * 
 */
UCLASS()
class FIRSTPERSONMODULE_API AFlashlight : public AEquipment
{
	GENERATED_BODY()
public:


	UPROPERTY(VisibleAnywhere, Category = "Rendering")
		class USkeletalMeshComponent* Mesh;





	//===========================================================================================================================================================================================================================
	//=========================================================================================================FUNCTIONS=========================================================================================================
	//===========================================================================================================================================================================================================================
public:
	AFlashlight(const FObjectInitializer& ObjectInitializer);



	
};
