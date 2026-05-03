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

UObject* UJournalSubsystem::PushPage(TSubclassOf<UUserWidget> PageType)
{
	if (Journal.GetObject())
		return IJournalInterface::Execute_PushPage(Journal.GetObject(), PageType);

	return nullptr;
}

UObject* UJournalSubsystem::PushPageEx(TSubclassOf<UUserWidget> PageType, const FString& EventID,
	bool bDelayUntilJournalOpen)
{
	if (Journal.GetObject())
		return IJournalInterface::Execute_PushPageEx(Journal.GetObject(), PageType, EventID, bDelayUntilJournalOpen);

	return nullptr;
}

TArray<UUserWidget*> UJournalSubsystem::PushPages(TArray<TSubclassOf<UUserWidget>> PageTypes)
{
	if (Journal.GetObject())
	{
		TArray<UUserWidget*> OutPages;
		for (const auto& PageType : PageTypes)
		{
			auto Page = IJournalInterface::Execute_PushPage(Journal.GetObject(), PageType);
			OutPages.Add(Page);
		}
		return OutPages;
	}

	return {};
}

UObject* UJournalSubsystem::PopPage(TSubclassOf<UUserWidget> PopAsType)
{
	if (Journal.GetObject())
		return IJournalInterface::Execute_PopPage(Journal.GetObject(), PopAsType);

	return nullptr;
}

TArray<UUserWidget*> UJournalSubsystem::GetPages()
{
	if (Journal.GetObject())
		return IJournalInterface::Execute_GetPages(Journal.GetObject());

	return {};
}

void UJournalSubsystem::ClearPages()
{
	if (Journal.GetObject()) IJournalInterface::Execute_ClearPages(Journal.GetObject());
}

void UJournalSubsystem::NavigateForward()
{
	if (Journal.GetObject()) IJournalInterface::Execute_NavigateForward(Journal.GetObject());
}

void UJournalSubsystem::NavigateBack()
{
	if (Journal.GetObject()) IJournalInterface::Execute_NavigateBack(Journal.GetObject());
}

void UJournalSubsystem::EnableNavigation(bool bAllowNavigation)
{
	if (Journal.GetObject()) IJournalInterface::Execute_EnableNavigation(Journal.GetObject(), bAllowNavigation);
}

void UJournalSubsystem::SetCanSkip(bool bCanSkip, UObject* LockedBy)
{
	if (Journal.GetObject()) IJournalInterface::Execute_SetCanSkip(Journal.GetObject(), bCanSkip, LockedBy);
}

UObject* UJournalSubsystem::PushPageAndStartPath(TSubclassOf<UUserWidget> InType, const FString& Path)
{
	if (Journal.GetObject())
	{
		UUserWidget* Page = IJournalInterface::Execute_PushPageEx(Journal.GetObject(), InType, Path, true);

		return Page;
	}

	return nullptr;
}

TArray<UUserWidget*> UJournalSubsystem::PushPagesAndStartPath(TArray<TSubclassOf<UUserWidget>> InTypes, const FString& Path)
{
	if (Journal.GetObject())
	{
		TArray<UUserWidget*> OutPages;
		for (const auto& PageType : InTypes)
		{
			auto Page = IJournalInterface::Execute_PushPageEx(Journal.GetObject(), PageType, Path, true);
			OutPages.Add(Page);
		}

		return OutPages;
	}

	return {};
}

void UJournalSubsystem::CloseJournal()
{
	if (Journal.GetObject())
		IJournalInterface::Execute_CloseJournal(Journal.GetObject());
}

int32 UJournalSubsystem::GetPageCount() const
{
	if (Journal.GetObject())
		return IJournalInterface::Execute_GetPageCount(Journal.GetObject());

	return 0;
}

bool UJournalSubsystem::IsPageFinalEventHandler(const UUserWidget* Page)
{
	if (Journal.GetObject())
		return IJournalInterface::Execute_IsPageFinalEventHandler(Journal.GetObject(), Page);

	return false;
}

bool UJournalSubsystem::IsPageActive(const UUserWidget* Page)
{
	if (!Page || GetPageCount() <= 0) return false;

	TArray<UUserWidget*> Pages = GetPages();
	if (Pages.Last() == Page) return true;

	if (GetPageCount() % 2 == 1 && GetPageCount() > 1 && Pages[GetPageCount() - 2] == Page) return true;

	return false;
}
