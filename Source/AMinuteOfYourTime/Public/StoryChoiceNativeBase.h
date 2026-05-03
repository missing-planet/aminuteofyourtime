// Copyright (c) 2025, Missing Planet LLC. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Components/Button.h"
#include "StoryChoiceNativeBase.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FChoiceSelectedSignature, int32, Index);

UINTERFACE(BlueprintType)
class UStoryChoiceInterface : public UInterface
{
	GENERATED_BODY()
};

class IStoryChoiceInterface
{
	GENERATED_BODY()

public:
	
	UFUNCTION(BlueprintImplementableEvent)
	void SetText(const FText& Text, const TArray<FString>& Tags);

	UFUNCTION(BlueprintImplementableEvent)
	void SetChoiceIndex(int32 Index);

	UFUNCTION(BlueprintImplementableEvent)
	int32 GetChoiceIndex();

	UFUNCTION(BlueprintImplementableEvent)
	void SetChoiceParent(UWidget* Parent);

	UFUNCTION(BlueprintImplementableEvent)
	void Reset();
};
