// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "StoryEvent.generated.h"

UCLASS(Blueprintable)
class AMINUTEOFYOURTIME_API UStoryEvent : public UObject
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable)
	void ExecuteEvent() {}
};
