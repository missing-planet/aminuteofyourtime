// Fill out your copyright notice in the Description page of Project Settings.


#include "EventDescriptionRow.h"
#include "StoryEventInterface.h"
#include "Blueprint/WidgetBlueprintLibrary.h"

UObject* UBlueprintEventFunctionLibrary::GetEvent(const FEventDescriptionRow& EventDescription, bool& Result)
{
	auto* Event = EventDescription.EventClass.GetDefaultObject();

	if (!Event || !Event->Implements<UStoryEventInterface>())
	{
		Result = false;
		return nullptr;
	}

	Result = true;
	return Event;
}

FEventDescriptionRow UBlueprintEventFunctionLibrary::GetEventForTime(const UDataTable* EventData, FDateTimePair DateTime)
{
	if (!EventData)
	{
		UE_LOG(LogTemp, Error, TEXT("Null Event table passed to GetEventForTime!"));
		return FEventDescriptionRow();
	}

	TArray<FEventDescriptionRow*> OutRows;
	EventData->GetAllRows<FEventDescriptionRow>(FString("Get Event For Time"), OutRows);
	for (const auto EventDescription : OutRows)
	{
		if (!EventDescription) continue;

		if (!EventDescription->EventTimes.Contains(DateTime)) continue;

		return *EventDescription;
	}

	return FEventDescriptionRow();
}
