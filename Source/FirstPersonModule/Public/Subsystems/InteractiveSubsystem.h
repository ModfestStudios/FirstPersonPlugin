// Copyrighted : Modfest Studios 2025-2026

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/WorldSubsystem.h"
#include "Tickable.h"
#include "Interactives/Interactives.h"
#include "InteractiveSubsystem.generated.h"

/**
 * 
 */
UCLASS()
class FIRSTPERSONMODULE_API UInteractiveSubsystem : public UTickableWorldSubsystem
{
	GENERATED_BODY()
public:


private:
	/*spatial-grid map that stores Interactive objects that can be evaluated in chunks*/	
	UPROPERTY()
		TMap<FIntPoint, FInteractiveGridEntry> InteractiveGrid;
	
	static constexpr int GridChunkSize = 10000;


protected:	
	UPROPERTY()
		TSet<TWeakObjectPtr<class UInteractiveComponent>> RegisteredInteractives;


protected:
	float GridRefreshInterval = 0.5f;
	float GridUpdateAccumulator = 0.0f;



	//=============================================================================================================================================================================
	//==================================================================================FUNCTIONS==================================================================================
	//=============================================================================================================================================================================

public:

	UInteractiveSubsystem();
		
	virtual TStatId GetStatId() const override;
	virtual void Tick(float DeltaTime) override;



	//========================================
	//==============REGISTRATION==============
	//========================================
public:
	UFUNCTION(BlueprintCallable, Category = "Interactives")
		virtual void RegisterInteractiveComponent(class UInteractiveComponent* Interactive);
	UFUNCTION(BlueprintCallable, Category = "Interactives")
		virtual void UnregisterInteractiveComponent(class UInteractiveComponent* Interactive);


	//========================================
	//===============SEARCHES=================
	//========================================
public:
	UFUNCTION(BlueprintCallable, Category = "Interactives")
		TArray<UInteractiveComponent*> GetNearbyInteractiveComponents(const FVector& Location, const float Threshold, const FInteractiveVicinityCheckParameters& VicinityCheckParameters) const;

	//========================================
	//==================GRID==================
	//========================================
protected:
	/*runs through all entries and make sure the interactives haven't moved - updating them if they have*/
	UFUNCTION()
		virtual void RefreshInteractiveGridKeys();
	UFUNCTION()
		virtual void UpdateInteractiveGridPosition(class UInteractiveComponent* Interactive);
	UFUNCTION()
		virtual FIntPoint GetGridKey(const FVector& Location) const;
	UFUNCTION()
		virtual FIntPoint GetGridKeyFromInteractive(class UInteractiveComponent* Interactive) const;


	UFUNCTION()
		virtual bool GridEntryExist(const FIntPoint& GridKey) const;
	UFUNCTION()
		FInteractiveGridEntry& GetGridEntry(const FIntPoint& GridKey);
	UFUNCTION()
		virtual void RemoveInteractiveFromGrid(const FIntPoint& GridKey, class UInteractiveComponent* Interactive);

};

