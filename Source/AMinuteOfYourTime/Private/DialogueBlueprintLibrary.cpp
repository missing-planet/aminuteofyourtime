// Fill out your copyright notice in the Description page of Project Settings.


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
