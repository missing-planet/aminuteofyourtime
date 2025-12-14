// Copyright (c) 2025, Missing Planet LLC. All rights reserved.

#pragma once

#include "CoreMinimal.h"

UENUM(BlueprintType)
enum class ECardType : uint8
{
	Basic        UMETA(DisplayName = "Basic"),
	Organ        UMETA(DisplayName = "Organ"),
	Appendage    UMETA(DisplayName = "Appendage"),
	Sense        UMETA(DisplayName = "Sense"),
	Information  UMETA(DisplayName = "Information"),
	Memory       UMETA(DisplayName = "Memory")
};
