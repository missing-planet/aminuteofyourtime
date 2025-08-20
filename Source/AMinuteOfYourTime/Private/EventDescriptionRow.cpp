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

UWidget* UBlueprintEventFunctionLibrary::GetHUDWidget(TSubclassOf<UUserWidget> WidgetClass, UObject* WorldContext)
{
	static TMap<TSubclassOf<UUserWidget>, UWidget*> CachedWidgets;

	if (CachedWidgets.Contains(WidgetClass)) return CachedWidgets[WidgetClass];

	TArray<UUserWidget*> FoundWidgets;
	UWidgetBlueprintLibrary::GetAllWidgetsOfClass(WorldContext, FoundWidgets, WidgetClass, false);

	if (!FoundWidgets.IsEmpty())
	{
		CachedWidgets.Add(WidgetClass, Cast<UWidget>(FoundWidgets[0]));
		return CachedWidgets[WidgetClass];
	}

	UE_LOG(LogTemp, Error, TEXT("No valid widget found!"));
	return nullptr;
}
