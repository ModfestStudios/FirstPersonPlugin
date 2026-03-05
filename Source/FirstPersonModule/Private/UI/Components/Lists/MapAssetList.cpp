// copyrighted 2024-2025 - Modfest Studios & Micah A. Parker


#include "UI/Components/Lists/MapAssetList.h"
#include "Components/ListView.h"

/*engine*/
#include "Engine/GameInstance.h"

/*maps*/
#include "Maps/MapAsset.h"

/*subsystems*/
#include "Subsystems/ModSubsystem.h"

void UMapAssetList::NativeConstruct()
{
	Super::NativeConstruct();

	RefreshMapList();
}

void UMapAssetList::RefreshMapList()
{
	/*clear old values*/
	Maps.Empty();

	if (ListView)
		ListView->ClearListItems();

	/*search for new maps*/
	if (UModSubsystem* ModSubsystem = GetGameInstance()->GetSubsystem<UModSubsystem>())
	{
		if (FilterCategory.IsNone())
			Maps = ModSubsystem->GetAllMaps();
		else
			Maps = ModSubsystem->GetMapsByCategory(FilterCategory);
	}

	/*auto-assigns any items to the list*/
	if (ListView)
	{
		for (int32 i = 0; i < Maps.Num(); i++)
		{
			ListView->AddItem(Maps[i]);
		}
	}
}
