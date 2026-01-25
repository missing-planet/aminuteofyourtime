// Copyright (c) 2025, Missing Planet LLC. All rights reserved.


#include "JournalSubsystem.h"

#include "InkStorySubsystem.h"
#include "Blueprint/UserWidget.h"
#include "Kismet/GameplayStatics.h"

UJournalSubsystem* UJournalSubsystem::GetJournalSubsystem(UObject* WorldContext)
{
	UGameInstance* GameInstance = UGameplayStatics::GetGameInstance(WorldContext);
	if (!GameInstance) return nullptr;

	return GameInstance->GetSubsystem<UJournalSubsystem>();
}

void UJournalSubsystem::SetJournalWidget(TScriptInterface<IJournalInterface> InJournal)
{
	Journal = InJournal;	
}

UObject* UJournalSubsystem::ChangePage(TSubclassOf<UUserWidget> InType)
{
	if (Journal)
		return Journal->ChangePage(InType);

	return nullptr;
}

UObject* UJournalSubsystem::ChangePageAndStartPath(TSubclassOf<UUserWidget> InType, const FString& Path)
{
	if (Journal.GetObject())
	{
		UUserWidget* Page = IJournalInterface::Execute_ChangePage(Journal.GetObject(), InType);
		UInkStorySubsystem* InkStory = GetGameInstance()->GetSubsystem<UInkStorySubsystem>();
		if (!InkStory) return Page;

		InkStory->SetCurrentStoryHandler(Page);
		InkStory->StartPath(Path);
		InkStory->ContinueStory();

		return Page;
	}

	return nullptr;
}

void UJournalSubsystem::CloseJournal()
{
	if (Journal.GetObject())
		IJournalInterface::Execute_CloseJournal(Journal.GetObject());
}
