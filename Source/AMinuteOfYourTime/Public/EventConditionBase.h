// Copyright (c) 2025, Missing Planet LLC. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "EventConditionBase.generated.h"

/**
 * 
 */
UCLASS(Abstract)
class AMINUTEOFYOURTIME_API UEventConditionBase : public UObject
{
	GENERATED_BODY()

public:

	bool CheckAvailability() { return false; }
};
