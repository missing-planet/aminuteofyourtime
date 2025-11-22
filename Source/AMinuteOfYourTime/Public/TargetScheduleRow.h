// Copyright (c) 2025, Missing Planet LLC. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Location.h"
#include "TargetScheduleRow.generated.h"

USTRUCT(BlueprintType)
struct FTargetScheduleRow : public FTableRowBase
{
	GENERATED_USTRUCT_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	ELocation Timeslot1Location = ELocation::Limbo;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	ELocation Timeslot2Location = ELocation::Limbo;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	ELocation Timeslot3Location = ELocation::Limbo;
};
