// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "EventDescriptionRow.h"
#include "UObject/Interface.h"
#include "StoryEventInterface.generated.h"

UINTERFACE(MinimalAPI, Blueprintable)
class UStoryEventInterface : public UInterface
{
	GENERATED_BODY()
};

class IStoryEventInterface
{
	GENERATED_BODY()

public:

	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable)
	void Execute(FEventDescriptionRow EventDescription, UObject* Caller);
};
