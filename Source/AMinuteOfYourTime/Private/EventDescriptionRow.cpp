// Fill out your copyright notice in the Description page of Project Settings.


#include "EventDescriptionRow.h"

#include "EventProbabilityRow.h"
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

TArray<FEventDescriptionRow> UBlueprintEventFunctionLibrary::GetEventsForTime(const UDataTable* EventData,
	FDateTimePair DateTime)
{
	if (!EventData)
	{
		UE_LOG(LogTemp, Error, TEXT("Null Event table passed to GetEventForTime!"));
		return {};
	}

	TArray<FEventDescriptionRow*> OutRows;
	TArray<FEventDescriptionRow> Result;
	EventData->GetAllRows<FEventDescriptionRow>(FString("Get Event For Time"), OutRows);
	for (FEventDescriptionRow* Row : OutRows)
	{
		if (Row && (Row->EventTimes.IsEmpty() || Row->EventTimes.Contains(DateTime)))
			Result.AddUnique(*Row);
	}

	return Result;
}

TArray<EventType> UBlueprintEventFunctionLibrary::GetUniqueEventTypes(const TArray<FEventDescriptionRow>& Events)
{
	TArray<EventType> Result;
	for (const FEventDescriptionRow& Row : Events)
	{
		Result.AddUnique(Row.EventType);
	}

	return Result;
}

TMap<EventType, float> UBlueprintEventFunctionLibrary::GetEventProbabilitiesForDay(const UDataTable* EventData, const FDateTimePair& DateTime)
{
	if (!EventData)
	{
		UE_LOG(LogTemp, Error, TEXT("Null Event table passed to GetEventProbabilitiesForDay!"));
		return {};
	}

	TArray<FEventProbabilityRow*> OutRows;
	EventData->GetAllRows<FEventProbabilityRow>(FString("Get Event Probabilities"), OutRows);
	for (FEventProbabilityRow* Row : OutRows)
	{
		if (Row && Row->Timeslot.Day == DateTime.Day) return Row->Probabilities;
	}

	UE_LOG(LogTemp, Error, TEXT("No overridden probabilities for Day %i found!"), DateTime.Day);
	return {};
}
