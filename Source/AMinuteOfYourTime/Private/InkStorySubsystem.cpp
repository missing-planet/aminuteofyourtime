// Fill out your copyright notice in the Description page of Project Settings.


#include "InkStorySubsystem.h"

#include "Inkpot/Inkpot.h"
#include "Inkpot/InkpotStory.h"
#include "Inkpot/Public/Inkpot/InkpotStory.h"

bool UInkStorySubsystem::StartStory(UInkpotStoryAsset* InStory)
{
	if (!InStory) return false;

	Story = GEngine->GetEngineSubsystem<UInkpot>()->BeginStory(InStory);

	return Story != nullptr;
 }

bool UInkStorySubsystem::ContinueStory()
{
	if (!Story) return false;

	if (!Story->CanContinue() && !Story->HasChoices())
	{
		PathEndReachedEvent.Broadcast(CurrentPath, CurrentStoryHandler.Get());
		CurrentPath = "";

		return false;
	} else if (Story->CanContinue())
	{
		Story->Continue();

		return true;
	}

	return false;
}

void UInkStorySubsystem::MakeChoice(UInkpotChoice* Choice)
{
	if (!Story || !Story->HasChoices()) return;

	Story->ChooseChoice(Choice);
}

void UInkStorySubsystem::MakeChoiceIndex(int32 Index)
{
	if (!Story || !Story->HasChoices()) return;

	Story->ChooseChoiceIndex(Index);
}

void UInkStorySubsystem::StartPath(const FString& Path)
{
	if (!Story) return;

	Story->ChoosePath(Path);
	CurrentPath = Path;

	PathStartedEvent.Broadcast(Path, CurrentStoryHandler.Get());
}

void UInkStorySubsystem::StartPathWithParameters(const FString& Path, const TArray<FInkpotValue>& Params)
{
	if (!Story) return;

	Story->ChoosePathString(Path, Params);
	CurrentPath = Path;

	PathStartedEvent.Broadcast(Path, CurrentStoryHandler.Get());
}
