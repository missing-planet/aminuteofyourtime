// Copyright (c) 2025, Missing Planet LLC. All rights reserved.


#include "EventDescriptionRow.h"

#include "EventProbabilityRow.h"
#include "StoryEventBase.h"
#include "StoryEventInterface.h"

TScriptInterface<IStoryEventInterface> UBlueprintEventFunctionLibrary::GetEvent(FEventDescriptionRow EventDescription,
	bool& Result)
{
	if (!EventDescription.EventClass)
	{
		Result = false;
		return nullptr;
	}
	
	UObject* DefaultObj = EventDescription.EventClass->GetDefaultObject();

	TScriptInterface<IStoryEventInterface> InterfaceWrapper;
	InterfaceWrapper.SetObject(DefaultObj);
	InterfaceWrapper.SetInterface(Cast<IStoryEventInterface>(DefaultObj));

	Result = true;
	return InterfaceWrapper;
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

TArray<FEventDescriptionRow> UBlueprintEventFunctionLibrary::GetAllowedEvents(const UDataTable* EventData)
{
	if (!EventData) return {};
	TArray<FEventDescriptionRow> Results;
	
	TArray<FEventDescriptionRow*> Events;
	EventData->GetAllRows(TEXT("UBlueprintEventFunctionLibrary::GetAllowedEvents"), Events);
	for (const FEventDescriptionRow* EventDescription : Events)
	{
		if (!EventDescription) continue;
		
		bool bAllowed = true;
		for (TSubclassOf<UEventConditionBase> ConditionClass : EventDescription->EventConditions)
		{
			if (!ConditionClass.GetDefaultObject()->CheckAvailability())
			{
				bAllowed = false;
				break;
			}
		}

		if (bAllowed) Results.AddUnique(*EventDescription);
	}

	return Results;
}

TArray<FEventDescriptionRow> UBlueprintEventFunctionLibrary::FilterEventsByType(
	const TArray<FEventDescriptionRow>& Events, EventType Type)
{
	TArray<FEventDescriptionRow> Result;

	for (const FEventDescriptionRow& Row : Events)
	{
		if (Row.EventType == Type) Result.Add(Row);
	}

	return Result;
}
