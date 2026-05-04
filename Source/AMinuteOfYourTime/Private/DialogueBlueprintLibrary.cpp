// Copyright (c) 2025, Missing Planet LLC. All rights reserved.


#include "DialogueBlueprintLibrary.h"

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
