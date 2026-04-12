// Copyright (c) 2025, Missing Planet LLC. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "BlueprintUtils.generated.h"

/**
 * 
 */
UCLASS()
class AMINUTEOFYOURTIME_API UBlueprintUtils : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()
	
	UFUNCTION(BlueprintPure)
	static UWidget* GetRootWidget(const UUserWidget* Widget)
	{
		return Widget->GetRootWidget();
	}
};
