// Copyrighted : Modfest Studios 2025-2026

#pragma once

#include "CoreMinimal.h"
#include "Inventory/InventoryItem.h"
#include "Battery.generated.h"

/**
 * 
 */
UCLASS()
class FIRSTPERSONMODULE_API ABattery : public AInventoryItem
{
	GENERATED_BODY()
public:

	
	UPROPERTY(VisibleAnywhere, Category = "Rendering")
		class UStaticMeshComponent* Mesh;



	//=================================================================================================================================================================================================================
	//====================================================================================================FUNCTIONS====================================================================================================
	//=================================================================================================================================================================================================================
public:
	ABattery(const FObjectInitializer& ObjectInitializer);
};
