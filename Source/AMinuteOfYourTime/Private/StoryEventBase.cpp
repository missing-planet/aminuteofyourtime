// Copyright (c) 2025, Missing Planet LLC. All rights reserved.


#include "StoryEventBase.h"
#include "InkStorySubsystem.h"
#include "TimeSubsystem.h"
#include "Kismet/GameplayStatics.h"

void UStoryEventBase::Execute_Implementation(FEventDescriptionRow EventDescription, UObject* Caller)
{
	UGameInstance* GameInstance = UGameplayStatics::GetGameInstance(Caller);
	if (!GameInstance) return;
	UInkStorySubsystem* InkStory = GameInstance->GetSubsystem<UInkStorySubsystem>();
	if (!InkStory) return;

	// TODO: Make this event actually provide the correct path name
	InkStory->EventEndReachedEvent.AddDynamic(this, &UStoryEventBase::OnPathEndReached);
}

void UStoryEventBase::OnPathEndReached_Implementation(const FString& PathName, UObject* CurrentHandler)
{
	if (PathName != EventPath) return;

	UTimeSubsystem* TimeSubsystem = GEngine->GetEngineSubsystem<UTimeSubsystem>();
	if (!TimeSubsystem) return;

	TimeSubsystem->IncreaseDay();
}