#pragma once
#include "EventType.h"
#include "Timeslot.h"

#include "EventProbabilityRow.generated.h"

USTRUCT(BlueprintType)
struct FEventProbabilityRow : public FTableRowBase
{
	GENERATED_USTRUCT_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FDateTimePair Timeslot;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TMap<EventType, float> Probabilities;
};
