// Copyright (c) 2025, Missing Planet LLC. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "SpeakerSettings.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "DialogueBlueprintLibrary.generated.h"

class UWidget;

UCLASS(Blueprintable)
class AMINUTEOFYOURTIME_API UDialogueBlueprintLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:

	UFUNCTION(BlueprintCallable, BlueprintPure)
	static float GetParameterValueFromLetter(const FString& Letter);

	UFUNCTION(BlueprintCallable, BlueprintPure)
	static FSpeakerSettings GetSpeakerSettingsFromTag(const UDataTable* SpeakerMap, const FString& Tag);

	UFUNCTION(BlueprintCallable, BlueprintPure)
	static FPortraitMapping GetPortraitFromTag(const UDataTable* PortraitMap, const FString& Tag);

	UFUNCTION(BlueprintCallable)
	static void SetLastExchangeResult(bool bAccepted);

	UFUNCTION(BlueprintCallable, BlueprintPure)
	static bool GetLastExchangeResult();

	UFUNCTION(BlueprintCallable)
	static bool DrawWidgetToTarget(UTextureRenderTarget2D * Target, UWidget* WidgetToRender, FVector2D DrawSize, bool UseGamma, TextureFilter Filter, float DeltaTime);
};
