/*this interface is intended to be used by any Actor that has an InventoryItemComponent attached to it - by using the interface the component will automatically call any relevant function
* whenever the component gets its calls - allowing you to get cleaner functions and feedback from your component without needing to bind to everything (which is intended more for external listeners
* rather than the owning actor itself and has a lot of redundant variables)
* by using this interface - your Actor gets "configured" automatically since the Component checks for the owning actor for the interface and calls the functions automatically
*/
// Copyright (c) 2022 Pocket Sized Animations

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
//#include "Components/InventoryItemComponent.h"
#include "InventoryItemInterface.generated.h"

// This class does not need to be modified.
UINTERFACE(MinimalAPI)
class UInventoryItemInterface : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class FIRSTPERSONMODULE_API IInventoryItemInterface
{
	GENERATED_BODY()

		friend class UInventoryItemComponent;

	// Add interface functions to this class. This is the class that will be inherited to implement this interface.
protected:

	
	UFUNCTION(BlueprintNativeEvent)
		void OnEquip();
	UFUNCTION(BlueprintNativeEvent)
		void OnUnequip();

	/*picking upping*/
	UFUNCTION(BlueprintnativeEvent)
		void OnPickedUp();


};
