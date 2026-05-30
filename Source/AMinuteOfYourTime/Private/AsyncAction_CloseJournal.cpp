// Copyright (c) 2025, Missing Planet LLC. All rights reserved.


#include "AsyncAction_CloseJournal.h"

#include "JournalSubsystem.h"
#include "JournalWidgetNativeBase.h"

UAsyncAction_CloseJournal* UAsyncAction_CloseJournal::CloseJournalAsync(UObject* WorldContextObject)
{
	UAsyncAction_CloseJournal* Action = NewObject<UAsyncAction_CloseJournal>();
	Action->WorldContext = WorldContextObject;
	return Action;
}

void UAsyncAction_CloseJournal::Activate()
{
	if (!WorldContext)
	{
		CleanUp();
		return;
	}

	UWorld* World = WorldContext->GetWorld();
	if (!World)
	{
		CleanUp();
		return;
	}

	UGameInstance* GI = World->GetGameInstance();
	if (!GI)
	{
		CleanUp();
		return;
	}

	UJournalSubsystem* JournalSubsystem = GI->GetSubsystem<UJournalSubsystem>();
	if (!JournalSubsystem)
	{
		CleanUp();
		return;
	}

	UJournalWidgetNativeBase* JournalWidget = JournalSubsystem->GetJournalWidget();
	if (!JournalWidget)
	{
		CleanUp();
		return;
	}

	JournalWidget->OnCloseFinished.AddUniqueDynamic(this, &UAsyncAction_CloseJournal::HandleCloseFinished);
	JournalWidget->OnClosedSkipped.AddUniqueDynamic(this, &UAsyncAction_CloseJournal::HandleCloseSkipped);

	JournalSubsystem->CloseJournal();
}

void UAsyncAction_CloseJournal::HandleCloseFinished()
{
	OnFinished.Broadcast();
	CleanUp();
}

void UAsyncAction_CloseJournal::HandleCloseSkipped()
{
	OnSkipped.Broadcast();
	CleanUp();
}

void UAsyncAction_CloseJournal::CleanUp() {if (WorldContext && WorldContext->GetWorld())
{
	if (UGameInstance* GI = WorldContext->GetWorld()->GetGameInstance())
	{
		if (UJournalSubsystem* JournalSubsystem = GI->GetSubsystem<UJournalSubsystem>())
		{
			if (UJournalWidgetNativeBase* JournalWidget = JournalSubsystem->GetJournalWidget())
			{
				// Unbind to prevent memory leaks or dangling pointers
				JournalWidget->OnCloseFinished.RemoveDynamic(this, &UAsyncAction_CloseJournal::HandleCloseFinished);
				JournalWidget->OnClosedSkipped.RemoveDynamic(this, &UAsyncAction_CloseJournal::HandleCloseSkipped);
			}
		}
	}
}

	// Tell Unreal this proxy object is done and can be garbage collected
	SetReadyToDestroy();
}
