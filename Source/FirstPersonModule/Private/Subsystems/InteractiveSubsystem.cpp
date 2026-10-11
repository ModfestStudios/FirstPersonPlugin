// Copyrighted : Modfest Studios 2025-2026


#include "Subsystems/InteractiveSubsystem.h"
#include "Components/InteractiveComponent.h"


UInteractiveSubsystem::UInteractiveSubsystem()
{
	
}

TStatId UInteractiveSubsystem::GetStatId() const
{
	RETURN_QUICK_DECLARE_CYCLE_STAT(UInteractiveSubsystem, STATGROUP_Tickables);
}

void UInteractiveSubsystem::Tick(float DeltaTime)
{	
	GridUpdateAccumulator += DeltaTime;

	if(GridUpdateAccumulator < GridRefreshInterval)
		return;

	/*reset timer for next update countdown*/
	GridUpdateAccumulator = 0.0f;

	/*scans through known interactives and ensures their positions are correct*/
	RefreshInteractiveGridKeys();
}

void UInteractiveSubsystem::RegisterInteractiveComponent(UInteractiveComponent* Interactive)
{
	/*register the interactive*/
	RegisteredInteractives.Add(Interactive);
	UpdateInteractiveGridPosition(Interactive);
}

void UInteractiveSubsystem::UnregisterInteractiveComponent(UInteractiveComponent* Interactive)
{
	if(!Interactive)
		return;


	const FIntPoint GridKey = GetGridKeyFromInteractive(Interactive);
	RemoveInteractiveFromGrid(GridKey, Interactive);
	RegisteredInteractives.Remove(Interactive);
}

TArray<UInteractiveComponent*> UInteractiveSubsystem::GetNearbyInteractiveComponents(const FVector& Location,const float Threshold,	const FInteractiveVicinityCheckParameters& VicinityCheckParameters) const
{
	TArray<UInteractiveComponent*> Results;

	if (Threshold <= 0.0f)
		return Results;

	/*Find every XY grid cell that could overlap the search radius*/
	const FVector MinLocation(Location.X - Threshold,Location.Y - Threshold,Location.Z);
	const FVector MaxLocation(Location.X + Threshold,Location.Y + Threshold,Location.Z);

	const FIntPoint MinGridKey = GetGridKey(MinLocation);
	const FIntPoint MaxGridKey = GetGridKey(MaxLocation);

	const float ThresholdSquared = FMath::Square(Threshold);

	for (int32 X = MinGridKey.X; X <= MaxGridKey.X; ++X)
	{
		for (int32 Y = MinGridKey.Y; Y <= MaxGridKey.Y; ++Y)
		{
			const FIntPoint GridKey(X, Y);

			const FInteractiveGridEntry* GridEntry = InteractiveGrid.Find(GridKey);
			if (!GridEntry)
				continue;

			for (const TWeakObjectPtr<UInteractiveComponent>& WeakInteractive : GridEntry->Interactives)
			{
				UInteractiveComponent* Interactive = WeakInteractive.Get();

				if (!IsValid(Interactive))
					continue;

				AActor* InteractiveOwner = Interactive->GetOwner();

				if (!IsValid(InteractiveOwner))
					continue;

				/*ignore requested actors*/
				if (VicinityCheckParameters.IgnoredActors.Contains(InteractiveOwner))
					continue;

				/*ignore disabled Interactives*/
				if (VicinityCheckParameters.bOnlyEnabledInteractives && !Interactive->IsInteractionEnabled())
					continue;

				const FVector InteractiveLocation = Interactive->GetComponentLocation();

				/*vertical restriction*/
				if (VicinityCheckParameters.MaxVerticalDistance > 0.0f)
				{
					const float VerticalDistance = FMath::Abs(InteractiveLocation.Z - Location.Z);

					if (VerticalDistance > VicinityCheckParameters.MaxVerticalDistance)
						continue;
				}

				/*distance restriction*/
				float DistanceSquared = 0.0f;

				if (VicinityCheckParameters.bUse2DDistance)
				{
					DistanceSquared = FVector::DistSquaredXY(Location, InteractiveLocation);
				}
				else
				{
					DistanceSquared = FVector::DistSquared(Location, InteractiveLocation);
				}

				if (DistanceSquared > ThresholdSquared)
					continue;

				/*visibility restriction*/
				if (VicinityCheckParameters.bPerformVisibilitySweep)
				{
					FHitResult HitResult;
					FCollisionQueryParams QueryParams;

					for (const TWeakObjectPtr<AActor>& WeakIgnoredActor : VicinityCheckParameters.IgnoredActors)
					{
						if (AActor* IgnoredActor = WeakIgnoredActor.Get())
						{
							QueryParams.AddIgnoredActor(IgnoredActor);
						}
					}

					const bool bBlockingHit = GetWorld()->LineTraceSingleByChannel(HitResult,Location,InteractiveLocation,ECC_Visibility,QueryParams);

					if (bBlockingHit && HitResult.GetActor() != InteractiveOwner)
						continue;
				}

				Results.Add(Interactive);
			}
		}
	}

	return Results;
}

void UInteractiveSubsystem::RefreshInteractiveGridKeys()
{
	for (auto It = RegisteredInteractives.CreateIterator(); It; ++It)
	{
		UInteractiveComponent* Interactive = It->Get();

		if (!IsValid(Interactive))
		{
			It.RemoveCurrent();
			continue;
		}

		UpdateInteractiveGridPosition(Interactive);
	}
}

void UInteractiveSubsystem::UpdateInteractiveGridPosition(UInteractiveComponent* Interactive)
{
	if (!IsValid(Interactive))
		return;

	const FIntPoint NewGridKey = GetGridKey(Interactive->GetComponentLocation());
	const FIntPoint OldGridKey = Interactive->GetGridKey();

	/*most common case - Interactive hasn't changed grid cells*/
	if (NewGridKey == OldGridKey)
	{
		if (const FInteractiveGridEntry* GridEntry = InteractiveGrid.Find(NewGridKey))
		{
			if (GridEntry->Interactives.Contains(Interactive))
				return;
		}
	}

	/*remove old grid reference*/
	if (NewGridKey != OldGridKey)
	{
		RemoveInteractiveFromGrid(OldGridKey, Interactive);
	}

	/*add new grid reference*/
	FInteractiveGridEntry& NewGridEntry = GetGridEntry(NewGridKey);
	NewGridEntry.Interactives.Add(Interactive);

	Interactive->UpdateGridKey(NewGridKey);
}

FIntPoint UInteractiveSubsystem::GetGridKey(const FVector& Location) const
{
	return FIntPoint(
		FMath::FloorToInt(Location.X / GridChunkSize),
		FMath::FloorToInt(Location.Y / GridChunkSize)
	);
}

FIntPoint UInteractiveSubsystem::GetGridKeyFromInteractive(UInteractiveComponent* Interactive) const
{
	if(!Interactive)
		return FIntPoint();

	return Interactive->GetGridKey();
}

bool UInteractiveSubsystem::GridEntryExist(const FIntPoint& GridKey) const
{
	return InteractiveGrid.Contains(GridKey);
}

FInteractiveGridEntry& UInteractiveSubsystem::GetGridEntry(const FIntPoint& GridKey)
{
	return InteractiveGrid.FindOrAdd(GridKey);
}

void UInteractiveSubsystem::RemoveInteractiveFromGrid(const FIntPoint& GridKey, UInteractiveComponent* Interactive)
{
	FInteractiveGridEntry* GridEntry = InteractiveGrid.Find(GridKey);	
	if(!GridEntry)
		return;

	/*remove the interactive from our list*/
	GridEntry->Interactives.Remove(Interactive);
	
	/*if we've removed all interactive references from a grid - go ahead and remove the grid entry itself to keep it lightweight*/
	if (GridEntry->Interactives.IsEmpty())	
		InteractiveGrid.Remove(GridKey);
	
}


