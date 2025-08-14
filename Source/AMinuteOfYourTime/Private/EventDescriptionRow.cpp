// Fill out your copyright notice in the Description page of Project Settings.


#include "EventDescriptionRow.h"

UStoryEvent* UBlueprintEventFunctionLibrary::GetEvent(const FEventDescriptionRow& EventDescription)
{
	return EventDescription.EventClass.GetDefaultObject();
}
