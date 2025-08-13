// Fill out your copyright notice in the Description page of Project Settings.

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
	void SetText(const FText& Text);

	UFUNCTION(BlueprintImplementableEvent)
	void SetChoiceIndex(int32 Index);

	UFUNCTION(BlueprintImplementableEvent)
	int32 GetChoiceIndex();

	UFUNCTION(BlueprintImplementableEvent)
	void SetChoiceParent(UWidget* Parent);

	UFUNCTION(BlueprintImplementableEvent)
	void Reset();
};
