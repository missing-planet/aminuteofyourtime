// Fill out your copyright notice in the Description page of Project Settings.


#include "StoryEventBase.h"
#include "InkStorySubsystem.h"
#include "TimeSubsystem.h"

void UStoryEventBase::Execute_Implementation(FEventDescriptionRow EventDescription, UObject* Caller)
{
	UInkStorySubsystem* InkStory = GEngine->GetEngineSubsystem<UInkStorySubsystem>();
	if (!InkStory) return;

	// TODO: Make this event actually provide the correct path name
	InkStory->EventEndReachedEvent.AddDynamic(this, &UStoryEventBase::OnPathEndReached);
}

void UStoryEventBase::OnPathEndReached_Implementation(const FString& PathName, UObject* CurrentHandler)
{
	if (PathName != EventPath) return;

	UTimeSubsystem* TimeSubsystem = GEngine->GetEngineSubsystem<UTimeSubsystem>();
	if (!TimeSubsystem) return;

	TimeSubsystem->IncreaseTime();
}