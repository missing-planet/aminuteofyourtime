// Copyright (c) 2025, Missing Planet LLC. All rights reserved.


#include "DialogueBlueprintLibrary.h"

#include "Components/Widget.h"
#include "Slate/WidgetRenderer.h"

float UDialogueBlueprintLibrary::GetParameterValueFromLetter(const FString& Letter)
{
	if (Letter.IsEmpty()) return -1.f;

	return (float)((int)Letter.ToUpper().GetCharArray()[0] - 65);
}

FSpeakerSettings UDialogueBlueprintLibrary::GetSpeakerSettingsFromTag(const UDataTable* SpeakerMap, const FString& Tag)
{
	if (!SpeakerMap) return FSpeakerSettings();

	if (auto Row = SpeakerMap->FindRow<FSpeakerSettings>(FName(Tag), "Get Speaker"))
	{
		return *Row;
	}

	return FSpeakerSettings();
}

FPortraitMapping UDialogueBlueprintLibrary::GetPortraitFromTag(const UDataTable* PortraitMap, const FString& Tag)
{
	if (!PortraitMap) return FPortraitMapping();

	if (auto Row = PortraitMap->FindRow<FPortraitMapping>(FName(Tag), "Get Portrait"))
	{
		return *Row;
	}

	return FPortraitMapping();
}

static bool bLastExchangeResult = false;

void UDialogueBlueprintLibrary::SetLastExchangeResult(bool bAccepted)
{
	bLastExchangeResult = bAccepted;
}

bool UDialogueBlueprintLibrary::GetLastExchangeResult()
{
	return bLastExchangeResult;
}

bool UDialogueBlueprintLibrary::DrawWidgetToTarget(UTextureRenderTarget2D* Target, UWidget* WidgetToRender,
	FVector2D DrawSize, bool UseGamma, TextureFilter Filter, float DeltaTime)
{
	// From https://github.com/pafuhana1213/RenderWidgetToTarget
	if (!WidgetToRender)
	{
		UE_LOG(LogTemp, Warning, TEXT("DrawWidgetToTarget Fail : WidgetToRender is empty!"));
		return false;
	}
	if (DrawSize.X < 0 || DrawSize.Y < 0)
	{
		UE_LOG(LogTemp, Warning, TEXT("DrawWidgetToTarget Fail : DrawSize is 0 or less!"));
		return false;
	}
	if (!Target)
	{
		UE_LOG(LogTemp, Warning, TEXT("DrawWidgetToTarget Fail : Target is empty!"));
		return false;
	}

	FWidgetRenderer* WidgetRenderer = new FWidgetRenderer(true, false);
	check(WidgetRenderer);

	TSharedRef<SWidget> ref = WidgetToRender->TakeWidget();
	WidgetRenderer->DrawWidget(Target, ref, DrawSize, DeltaTime);
	FlushRenderingCommands();

	BeginCleanup(WidgetRenderer);

	return true;
}
