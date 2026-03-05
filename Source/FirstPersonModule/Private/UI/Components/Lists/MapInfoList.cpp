// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/Components/Lists/MapInfoList.h"
#include "Components/ListView.h"

/*maps*/
#include "Maps/MapAsset.h"

/*subsystems*/
#include "Subsystems/ModSubsystem.h"

void UMapInfoList::NativeConstruct()
{
	Super::NativeConstruct();

	//RefreshMapList();	
}
//
//void UMapInfoList::RefreshMapList()
//{
//	/*clear old values*/
//	Maps.Empty();
//
//	if (ListView)
//		ListView->ClearListItems();
//
//	/*search for new maps*/
//	if (UModSubsystem* ModSubsystem = GetGameInstance()->GetSubsystem<UModSubsystem>())
//	{
//		if (FilterCategory.IsNone())
//			Maps = ModSubsystem->GetAllMaps();
//		else
//			Maps = ModSubsystem->GetMapsByCategory(FilterCategory);
//	}
//
//	/*auto-assigns any items to the list*/
//	if (ListView)
//	{
//		for (int32 i = 0; i < Maps.Num(); i++)
//		{
//			ListView->AddItem(Maps[i]);
//		}
//	}
//}
