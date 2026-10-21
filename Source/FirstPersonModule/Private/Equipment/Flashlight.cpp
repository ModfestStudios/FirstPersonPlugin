// Copyrighted : Modfest Studios 2025-2026


#include "Equipment/Flashlight.h"

AFlashlight::AFlashlight(const FObjectInitializer& ObjectInitializer)
	:Super(ObjectInitializer)
{
	Mesh = ObjectInitializer.CreateDefaultSubobject<USkeletalMeshComponent>(this, TEXT("Mesh"));
}
