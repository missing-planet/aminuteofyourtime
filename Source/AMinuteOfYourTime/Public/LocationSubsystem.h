// Copyright (c) 2025, Missing Planet LLC. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "EventType.h"
#include "Location.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "LocationSubsystem.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FProbabilitiesChangedSignature, EventType, Event, float, Probability);

UCLASS()
class AMINUTEOFYOURTIME_API ULocationSubsystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()

public:

	virtual void Initialize(FSubsystemCollectionBase& Collection) override;

	UFUNCTION(BlueprintCallable, BlueprintPure)
	// Gets event probability for the given location, optionally including the current offsets
	float GetLocationProbabilityForEventType(ELocation Location, EventType Event, bool bIncludeOffsets = true) const;

	UFUNCTION(BlueprintCallable, BlueprintPure)
	// Gets all event probabilities for the given location, optionally including the current offsets
	TMap<EventType, float> GetEventProbabilitiesForLocation(ELocation Location, bool bIncludeOffsets = true) const;

	UFUNCTION(BlueprintCallable, BlueprintPure)
	// Gets event probability offset for the current location
	float GetCurrentOffsetForEventType(EventType Event) const;

	UFUNCTION(BlueprintCallable, BlueprintPure)
	// Gets all event probability offsets for the current location
	const TMap<EventType, float>& GetCurrentOffsetsForEvent() const;

	UFUNCTION(BlueprintCallable, BlueprintPure)
	// Gets permanent event probability offset for the current location
	float GetPermanentOffsetForEventType(EventType Event) const;

	UFUNCTION(BlueprintCallable, BlueprintPure)
	// Gets all permanent event probability offsets for the current location
	const TMap<EventType, float>& GetPermanentEventOffset() const;

	UFUNCTION(BlueprintCallable)
	// Adds Delta to the probability for the given location and event type, then returns the new total.
	// bOffsetInstead will apply Delta to the current location offsets, ignoring Location
	float UpdateLocationProbabilityForEventType(ELocation Location, EventType Event, float Delta,
		bool bOffsetInstead = false);

	UFUNCTION(BlueprintCallable)
	// For each event type in Deltas it adds the delta to that event type's probability at the given location
	// bOffsetInstead will apply Deltas to the current location offsets, ignoring Location
	void UpdateLocationProbabilities(ELocation Location, const TMap<EventType, float>& Deltas,
		bool bOffsetInstead = false);

	UFUNCTION(BlueprintCallable)
	// Same as Update, but it just resets the probability to Value
	// bOffsetInstead will apply Value to the current location offset, ignoring Location
	void SetLocationProbabilityForEventType(ELocation Location, EventType Event, float Value,
		bool bOffsetInstead = false, bool bPermanentInstead = false);

	UFUNCTION(BlueprintCallable)
	// Same as Update, but it just resets the probability to Value
	// bOffsetInstead will apply Values to the current location offsets, ignoring Location
	void SetLocationProbabilities(ELocation Location, const TMap<EventType, float>& Values,
		bool bOffsetInstead = false, bool bPermanentInstead = false);

	UFUNCTION(BlueprintCallable)
	// Adds Delta to the current location's offsets for the given Event, then adjusts the other
	// offsets so that when they're added to the location's base probabilities, the probabilities will
	// still sum to 100.
	// e.g. if you add 30% to Lore, 30% will be deducted from the other events, weighted so the lowest
	// probabilities lose the least
	void AddAutoAdjustingOffset(EventType Event, float Delta, bool bIsPermanent = false);

	UFUNCTION(BlueprintCallable)
	// Takes a list of currently allowed events and returns a map of probabilities scaled to 100%
	TMap<EventType, float> GetNormalizedProbabilitiesForAvailableEvents(ELocation Location,
		const TArray<EventType>& AvailableEvents) const;

	UFUNCTION(BlueprintCallable, BlueprintPure)
	EventType PickEventType(ELocation Location, const TArray<EventType>& AvailableEvents) const;

	UFUNCTION(BlueprintCallable)
	// Adds permanent offsets into the base probabilities and clears both offsets
	void ApplyOffsets();

	UFUNCTION(BlueprintCallable)
	void ClearOffsets(bool bIncludePermanent = false);

	UFUNCTION(BlueprintCallable)
	void ClearPermanentOffsets();

	UFUNCTION(BlueprintCallable)
	void SetCurrentLocation(ELocation NewLocation);

	UFUNCTION(BlueprintCallable)
	void ClearCurrentLocation();

	UPROPERTY(BlueprintAssignable)
	FProbabilitiesChangedSignature ProbabilitiesChangedEvent;

private:

	TOptional<ELocation> CurrentLocation;

	TMap<ELocation, TMap<EventType, float>> LocationProbabilities;

	// Offsets are accumulated as cards are played (or removed), counted for the sacrifice,
	// then cleared once it's been evaluated
	TMap<EventType, float> CurrentLocationOffsets;

	// Permanent offsets are the same as normal offsets, but after the sacrifice has been evaluated
	// they get permanently applied to the current location's main probabilities in LocationProbabilities.
	// This is mainly so we can have card effects that temporarily affect probabilities, and card effects
	// that permanently affect them. If you just want to permanently increase probabilities without involving
	// card effects, use the Update or Set functions above.
	TMap<EventType, float> CurrentLocationPermanentOffsets;
};

// Wrapper struct to make the probabilities editable in the editor, since UHT won't allow
// nested containers as UPROPERTIES otherwise
USTRUCT(BlueprintType)
struct FEventProbabilityWrapper
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TMap<EventType, float> EventProbabilities;
};

UCLASS()
class AMINUTEOFYOURTIME_API UDefaultProbabilitiesDataAsset : public UDataAsset
{
	GENERATED_BODY()

public:

	UPROPERTY(EditAnywhere)
	TMap<ELocation, FEventProbabilityWrapper> DefaultLocationProbabilities;
	
};
