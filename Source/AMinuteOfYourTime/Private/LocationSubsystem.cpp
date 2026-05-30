// Copyright (c) 2025, Missing Planet LLC. All rights reserved.


#include "LocationSubsystem.h"

#include "ProjectSettings.h"

static const TMap<EventType, float> EmptyEventMap;

void ULocationSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);

	const ULocationProjectSettings* Settings = GetDefault<ULocationProjectSettings>();
	if (!Settings) return;

	if (Settings->DefaultProbabilityDataAsset.IsNull())
	{
		UE_LOG(LogTemp, Warning, TEXT("LocationSubsystem: No Data Asset selected in Project Settings!"));
		return;
	}

	UDefaultProbabilitiesDataAsset* ProbabilityConfig = Settings->DefaultProbabilityDataAsset.LoadSynchronous();
	
	if (!ProbabilityConfig)
	{
		UE_LOG(LogTemp, Error, TEXT("LocationSubsystem: Data Asset failed to load synchronously!"));
		return;
	}

	LocationProbabilities.Empty();
	for (const auto& LocationKvp : ProbabilityConfig->DefaultLocationProbabilities)
	{
		ELocation Location = LocationKvp.Key;
		const FEventProbabilityWrapper& Wrapper = LocationKvp.Value;

		TMap<EventType, float>& RuntimeSubMap = LocationProbabilities.FindOrAdd(Location);
		for (const auto& EventKvp : Wrapper.EventProbabilities)
		{
			RuntimeSubMap.Add(EventKvp.Key, EventKvp.Value);
		}
	}
	
	UE_LOG(LogTemp, Log, TEXT("LocationSubsystem: Successfully loaded baseline probabilities!"));
}

float ULocationSubsystem::GetLocationProbabilityForEventType(ELocation Location, EventType Event, bool bIncludeOffsets) const
{
	float BaseProbability = 0.0f;
	if (LocationProbabilities.Contains(Location) && LocationProbabilities[Location].Contains(Event))
	{
		BaseProbability = LocationProbabilities[Location][Event];
	}

	if (!bIncludeOffsets)
	{
		return BaseProbability;
	}

	float Offset = GetCurrentOffsetForEventType(Event);
	float PermOffset = GetPermanentOffsetForEventType(Event);

	// Clamp final probability tightly between 0% and 100%
	return FMath::Clamp(BaseProbability + Offset + PermOffset, 0.0f, 100.0f);
}

TMap<EventType, float> ULocationSubsystem::GetEventProbabilitiesForLocation(ELocation Location, bool bIncludeOffsets) const
{
	if (!bIncludeOffsets)
	{
		return LocationProbabilities.Contains(Location) ? LocationProbabilities[Location] : EmptyEventMap;
	}
	
	TMap<EventType, float> TotalMap;
	TotalMap.Empty();

	// Make sure we get values for ALL event types in the various maps
	TSet<EventType> AllEvents;
	if (LocationProbabilities.Contains(Location))
	{
		LocationProbabilities[Location].GetKeys(AllEvents);
	}
	CurrentLocationOffsets.GetKeys(AllEvents);
	CurrentLocationPermanentOffsets.GetKeys(AllEvents);

	for (const EventType& Event : AllEvents)
	{
		TotalMap.Add(Event, GetLocationProbabilityForEventType(Location, Event, true));
	}

	return TotalMap;
}

float ULocationSubsystem::GetCurrentOffsetForEventType(EventType Event) const
{
	return CurrentLocationOffsets.Contains(Event) ? CurrentLocationOffsets[Event] : 0.0f;
}

const TMap<EventType, float>& ULocationSubsystem::GetCurrentOffsetsForEvent() const
{
	return CurrentLocationOffsets;
}

float ULocationSubsystem::GetPermanentOffsetForEventType(EventType Event) const
{
	return CurrentLocationPermanentOffsets.Contains(Event) ? CurrentLocationPermanentOffsets[Event] : 0.0f;
}

const TMap<EventType, float>& ULocationSubsystem::GetPermanentEventOffset() const
{
	return CurrentLocationPermanentOffsets;
}

float ULocationSubsystem::UpdateLocationProbabilityForEventType(ELocation Location, EventType Event, float Delta, bool bOffsetInstead)
{
	if (bOffsetInstead)
	{
		float& TargetOffset = CurrentLocationOffsets.FindOrAdd(Event, 0.0f);
		TargetOffset += Delta;
		
		float NewTotal = GetLocationProbabilityForEventType(Location, Event, true);
		ProbabilitiesChangedEvent.Broadcast(Event, NewTotal);
		return NewTotal;
	}

	// Standard update to base location profile
	TMap<EventType, float>& EventMap = LocationProbabilities.FindOrAdd(Location);
	float& BaseValue = EventMap.FindOrAdd(Event, 0.0f);
	BaseValue = FMath::Clamp(BaseValue + Delta, 0.0f, 100.0f);

	float NewTotal = GetLocationProbabilityForEventType(Location, Event, true);
	ProbabilitiesChangedEvent.Broadcast(Event, NewTotal);
	return NewTotal;
}

void ULocationSubsystem::UpdateLocationProbabilities(ELocation Location, const TMap<EventType, float>& Deltas, bool bOffsetInstead)
{
	for (const auto& Kvp : Deltas)
	{
		UpdateLocationProbabilityForEventType(Location, Kvp.Key, Kvp.Value, bOffsetInstead);
	}
}

void ULocationSubsystem::SetLocationProbabilityForEventType(ELocation Location, EventType Event, float Value,
	bool bOffsetInstead, bool bPermanentInstead)
{
	if (bOffsetInstead)
	{
		bPermanentInstead ? CurrentLocationPermanentOffsets.FindOrAdd(Event, 0.0f) = Value
			: CurrentLocationOffsets.FindOrAdd(Event, 0.0f) = Value;
		ProbabilitiesChangedEvent.Broadcast(Event, GetLocationProbabilityForEventType(Location, Event, true));
		return;
	}

	TMap<EventType, float>& EventMap = LocationProbabilities.FindOrAdd(Location);
	EventMap.FindOrAdd(Event, 0.0f) = FMath::Clamp(Value, 0.0f, 100.0f);
	
	ProbabilitiesChangedEvent.Broadcast(Event, GetLocationProbabilityForEventType(Location, Event, true));
}

void ULocationSubsystem::SetLocationProbabilities(ELocation Location, const TMap<EventType, float>& Values,
	bool bOffsetInstead, bool bPermanentInstead)
{
	for (const auto& Kvp : Values)
	{
		SetLocationProbabilityForEventType(Location, Kvp.Key, Kvp.Value, bOffsetInstead, bPermanentInstead);
	}
}

void ULocationSubsystem::AddAutoAdjustingOffset(EventType Event, float Delta, bool bIsPermanent)
{
	// Target map choice based on tracking type
	TMap<EventType, float>& TargetMap = bIsPermanent ? CurrentLocationPermanentOffsets : CurrentLocationOffsets;
	
	// Apply the delta to our target choice directly
	float& TargetValue = TargetMap.FindOrAdd(Event, 0.0f);
	TargetValue += Delta;

	// To auto-adjust dynamically, we look at all OTHER events currently populated in the system
	TArray<EventType> OtherEvents;
	float TotalOtherWeights = 0.0f;

	check(CurrentLocation.IsSet());
	ELocation CurrLocation = CurrentLocation.GetValue();

	// Calculate current baseline distribution of other elements to extract our adjustment weights
	for (const auto& Kvp : CurrentLocationOffsets)
	{
		if (Kvp.Key != Event)
		{
			OtherEvents.Add(Kvp.Key);
			TotalOtherWeights += GetLocationProbabilityForEventType(CurrLocation, Kvp.Key, true);
		}
	}

	if (OtherEvents.Num() == 0 || TotalOtherWeights <= 0.0f)
	{
		// Fallback: Broadcast change if there are no other channels to deduct fractions from
		ProbabilitiesChangedEvent.Broadcast(Event, GetLocationProbabilityForEventType(CurrLocation, Event, true));
		return;
	}

	// Distribute the inverse penalty across other active event streams.
	// Higher current probabilities absorb a larger portion of the reduction, leaving lower values safer.
	for (const EventType& OtherEvent : OtherEvents)
	{
		float CurrentProbability = GetLocationProbabilityForEventType(CurrLocation, OtherEvent, true);
		float ProportionalShare = (CurrentProbability / TotalOtherWeights) * Delta;
		
		// Subtract the proportional share from the secondary options
		float& OtherOffset = TargetMap.FindOrAdd(OtherEvent, 0.0f);
		OtherOffset -= ProportionalShare;

		ProbabilitiesChangedEvent.Broadcast(OtherEvent, GetLocationProbabilityForEventType(CurrLocation, OtherEvent, true));
	}

	ProbabilitiesChangedEvent.Broadcast(Event, GetLocationProbabilityForEventType(CurrLocation, Event, true));
}

TMap<EventType, float> ULocationSubsystem::GetNormalizedProbabilitiesForAvailableEvents(ELocation Location,
	const TArray<EventType>& AvailableEvents) const
{
	TMap<EventType, float> OutputProbabilities;

	if (AvailableEvents.Num() == 0)
	{
		return OutputProbabilities;
	}

	float TotalActiveWeight = 0.0f;
	TMap<EventType, float> RawCurrentProbabilities;

	// 1. Gather the true current probabilities (Base + Offsets) ONLY for available events
	for (const EventType& Event : AvailableEvents)
	{
		float CurrentTrueProb = GetLocationProbabilityForEventType(Location, Event, true);
		
		RawCurrentProbabilities.Add(Event, CurrentTrueProb);
		TotalActiveWeight += CurrentTrueProb;
	}

	// 2. Normalize the subset up to 100%
	if (TotalActiveWeight > 0.0f)
	{
		for (const auto& Kvp : RawCurrentProbabilities)
		{
			// Scale proportionally: (Individual Weight / Subset Total Weight) * 100
			float NormalizedProbability = (Kvp.Value / TotalActiveWeight) * 100.0f;
			OutputProbabilities.Add(Kvp.Key, NormalizedProbability);
		}
	}
	else
	{
		// Fallback: If all available events currently have a 0% true probability, 
		// distribute chances completely evenly among the available subset
		float EvenSplit = 100.0f / AvailableEvents.Num();
		for (const EventType& Event : AvailableEvents)
		{
			OutputProbabilities.Add(Event, EvenSplit);
		}
	}

	return OutputProbabilities;
}

EventType ULocationSubsystem::PickEventType(ELocation Location, const TArray<EventType>& AvailableEvents) const
{
	const float RandomValue = FMath::RandRange(0.0f, 100.0f);
	float Accumulator = 0.0f;

	TMap<EventType, float> Probabilities = GetNormalizedProbabilitiesForAvailableEvents(Location, AvailableEvents);

	for (const auto& [Event, Probability] : Probabilities )
	{
		Accumulator += Probability;
		if (RandomValue <= Accumulator) return Event;
	}

	UE_LOG(LogTemp, Error,
		TEXT("Didn't find a valid event type for location %s. Make sure your probabilities sum to 100."),
		*StaticEnum<ELocation>()->GetDisplayNameText().ToString());
	return EventType::Lore;
}

void ULocationSubsystem::ApplyOffsets()
{
	check(CurrentLocation.IsSet());
	ELocation CurrLocation = CurrentLocation.GetValue();
	
	// Merge Permanent Offsets directly down into the persistent Base Location storage
	TMap<EventType, float>& BaseEventMap = LocationProbabilities.FindOrAdd(CurrLocation);

	for (const auto& Kvp : CurrentLocationPermanentOffsets)
	{
		float& BaseValue = BaseEventMap.FindOrAdd(Kvp.Key, 0.0f);
		BaseValue = FMath::Clamp(BaseValue + Kvp.Value, 0.0f, 100.0f);
	}

	// Completely wipe out our active transient and permanent card modifier registers
	ClearOffsets(true);
}

void ULocationSubsystem::ClearOffsets(bool bIncludePermanent)
{
	check(CurrentLocation.IsSet());
	ELocation CurrLocation = CurrentLocation.GetValue();
	
	CurrentLocationOffsets.Empty();
	if (bIncludePermanent)
	{
		ClearPermanentOffsets();
	}
	
	// Broadcast updates to notify UI/systems that baselines have restored
	for (const auto& Kvp : LocationProbabilities.FindOrAdd(CurrLocation))
	{
		ProbabilitiesChangedEvent.Broadcast(Kvp.Key, Kvp.Value);
	}
}

void ULocationSubsystem::ClearPermanentOffsets()
{
	CurrentLocationPermanentOffsets.Empty();
}

void ULocationSubsystem::SetCurrentLocation(ELocation NewLocation)
{
	CurrentLocation = NewLocation;

	ClearOffsets(true);

	TMap<EventType, float> EmptyOffsets = {
		{EventType::Lore, 0.0f},
		{EventType::Exchange, 0.0f},
		{EventType::Clue, 0.0f},
		{EventType::Other, 0.0f},
	};
	SetLocationProbabilities(NewLocation, EmptyOffsets, true);
	SetLocationProbabilities(NewLocation, EmptyOffsets, true, true);
}

void ULocationSubsystem::ClearCurrentLocation()
{
	CurrentLocation.Reset();
}
