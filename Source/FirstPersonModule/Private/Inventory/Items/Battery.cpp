// Copyrighted : Modfest Studios 2025-2026


#include "Inventory/Items/Battery.h"
#include "Components/InteractiveComponent.h"
#include "Components/InventoryItemComponent.h"
#include "Components/StaticMeshComponent.h"

ABattery::ABattery(const FObjectInitializer& ObjectInitializer)
	:Super(ObjectInitializer)
{
	if (InteractiveComponent)
		InteractiveComponent->CollisionLineThickness = 1.0f;

	Mesh = ObjectInitializer.CreateDefaultSubobject<UStaticMeshComponent>(this, TEXT("Mesh"));

	if (Mesh)
	{
		Mesh->SetupAttachment(GetRootComponent());
	}

	if (ItemComponent)
	{
		ItemComponent->Size = EItemSize::Small;
	}
}
