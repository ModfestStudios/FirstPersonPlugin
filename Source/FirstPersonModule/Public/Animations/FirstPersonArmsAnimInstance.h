// Copyright (c) 2022 Pocket Sized Animations

/*First Person Arms Anim Instance - This class is designed to be more of a template than an out-of-the-box solution.
* it provides a lot of functions you can override to make a first person setup easier
* check out ExtendedFirstPersonModule for a version that uses most of the other Endurance Modules (used in games such as Black Sierra and Tier One) for a more out of the box approach
* (assuming you're using those modules)
*
*/

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "Animations/FirstPersonAnimInstance.h"
#include "FirstPersonArmsAnimInstance.generated.h"



/**
 *
 */
UCLASS(abstract)
class FIRSTPERSONMODULE_API UFirstPersonArmsAnimInstance : public UFirstPersonAnimInstance
{
	GENERATED_BODY()
protected:


};
