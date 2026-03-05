// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Components/ListView.h"
#include "Components/ListViewBase.h"
#include "Widgets/Views/SListView.h"
#include "CharacterInfoList.generated.h"

UENUM(BlueprintType)
enum class ECharacterInfoListType : uint8
{
	AllCharacters,
	UnselectedCharacters,
	UnassignedTeamMembers,
	AssignedTeamMembers
};

/**
 * 
 */
UCLASS()
class FIRSTPERSONMODULE_API UCharacterInfoList : public UUserWidget
{
	GENERATED_BODY()
public:

	//UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Characters")
		//bool bSelectableCharactersOnly;

	/*Auto-Binds to a ListView of the same name to allow C++ driven updates*/
	UPROPERTY(meta = (BindWidget))
		UListView* LIST_CharacterList;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Characters")
		ECharacterInfoListType CharacterSelectionType = ECharacterInfoListType::AllCharacters;

	UPROPERTY(BlueprintReadWrite, Category = "Characters")
		TArray<class ACharacterInfo*> Characters;


	//===========================================================================================================================================
	//=================================================================FUNCTIONS=================================================================
	//===========================================================================================================================================
	
	virtual void NativeOnInitialized() override;
	virtual void NativeConstruct() override;


	UFUNCTION()
		virtual void OnSelectionChanged(UObject* Item);
	UFUNCTION()
		virtual void OnCharacterListChanged();

	UFUNCTION(BlueprintImplementableEvent, Category = "Characters", meta = (DisplayName = "On Refresh Character List"))
		 void BP_OnRefreshCharacterList();

	UFUNCTION(BlueprintCallable, Category = "Characters")
		virtual void RefreshCharacterList();




	
};
