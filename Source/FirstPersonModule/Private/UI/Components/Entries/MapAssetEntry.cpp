// copyrighted 2024-2025 - Modfest Studios & Micah A. Parker


#include "UI/Components/Entries/MapAssetEntry.h"
#include "Maps/MapAsset.h"
#include "Subsystems/MapSubsystem.h"

/*engine*/
#include "Engine/GameInstance.h"

void UMapAssetEntry::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);

}

void UMapAssetEntry::NativeOnListItemObjectSet(UObject* ListItemObject)
{
	if (UMapAsset* Map = Cast<UMapAsset>(ListItemObject))
		MapInfo = Map;

	Execute_OnListItemObjectSet(Cast<UObject>(this), ListItemObject);
}

void UMapAssetEntry::NativeOnItemSelectionChanged(bool bIsSelected)
{
	if (bIsSelected)
	{
		if (bAddToMapRotationOnSelection)
			AddToMapRotation();
		else
			SetAsNextMap();
	}

	Execute_BP_OnItemSelectionChanged(Cast<UObject>(this), bIsSelected);
}

void UMapAssetEntry::SetAsNextMap()
{
	if (!MapInfo)
		return;

	if (UMapSubsystem* MapSubsystem = GetGameInstance()->GetSubsystem<UMapSubsystem>())
		MapSubsystem->SetNextMap(MapInfo);
}

void UMapAssetEntry::AddToMapRotation()
{
	if (!MapInfo)
		return;

	if (UMapSubsystem* MapSubsystem = GetGameInstance()->GetSubsystem<UMapSubsystem>())
		MapSubsystem->AddToMapRotation(MapInfo);
}
