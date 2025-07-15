// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "SpeakerSettings.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "DialogueBlueprintLibrary.generated.h"

UCLASS(Blueprintable)
class AMINUTEOFYOURTIME_API UDialogueBlueprintLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:

	UFUNCTION(BlueprintCallable, BlueprintPure)
	static float GetParameterValueFromLetter(const FString& Letter);

	UFUNCTION(BlueprintCallable, BlueprintPure)
	static FSpeakerSettings GetSpeakerSettingsFromTag(const UDataTable* SpeakerMap, const FString& Tag);
};
