// Fill out your copyright notice in the Description page of Project Settings.


#include "DialogueWidgetBase.h"

#include "InkStorySubsystem.h"
#include "StoryChoiceNativeBase.h"
#include "Inkpot/Inkpot.h"
#include "Kismet/GameplayStatics.h"

void UDialogueWidgetBase::NativePreConstruct()
{
	Super::NativePreConstruct();

	if (CursorAnimation) PlayAnimation(CursorAnimation, 0, 0);

	UInkpot* InkpotSystem = GEngine->GetEngineSubsystem<UInkpot>();
	UInkStorySubsystem* InkStory = GEngine->GetEngineSubsystem<UInkStorySubsystem>();
	if (!InkpotSystem || !InkStory) return;

	InkpotSystem->EventOnStoryBegin.AddDynamic(this, &UDialogueWidgetBase::OnBeginStory);

	InkStory->PathStartedEvent.AddDynamic(this, &UDialogueWidgetBase::OnPathStarted);
	InkStory->PathEndReachedEvent.AddDynamic(this, &UDialogueWidgetBase::OnPathEndReached);
}

void UDialogueWidgetBase::NativeConstruct()
{
	Super::NativeConstruct();

	Hide();

	Text_Cursor->SetVisibility(ESlateVisibility::Collapsed);
}

FReply UDialogueWidgetBase::NativeOnMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
	if (InMouseEvent.GetEffectingButton() == FKey("LeftMouseButton"))
	{
		UInkStorySubsystem* InkStory = GEngine->GetEngineSubsystem<UInkStorySubsystem>();
		if (!InkStory || !bAllowSkip) return FReply::Unhandled();

		if (bIsShowingLine)
		{
			OnSkipLine();
			LineProgress = CurrentLine.ToString().Len();
			bIsShowingLine = false;
			Text_Cursor->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
			UpdateChoicesView(InkStory->GetStory());

			return FReply::Handled();
		} else
		{
			if (InkStory->ContinueStory()) Text_Cursor->SetVisibility(ESlateVisibility::Collapsed);

			return FReply::Handled();
		}
	}

	return FReply::Unhandled();
}

FReply UDialogueWidgetBase::NativeOnMouseButtonDoubleClick(const FGeometry& InGeometry,
	const FPointerEvent& InMouseEvent)
{
	if (InMouseEvent.GetEffectingButton() == FKey("LeftMouseButton"))
	{
		if (!bAllowSkip) return FReply::Unhandled();

		if (bIsShowingLine) OnSkipLine();
		LineProgress = CurrentLine.ToString().Len();
		bIsShowingLine = false;

		UInkStorySubsystem* InkStory = GEngine->GetEngineSubsystem<UInkStorySubsystem>();
		if (!InkStory) return FReply::Handled();

		if (InkStory->ContinueStory()) Text_Cursor->SetVisibility(ESlateVisibility::Collapsed);

		return FReply::Handled();
	}

	return FReply::Unhandled();
}

void UDialogueWidgetBase::OnPathStarted_Implementation(const FString& PathName, UObject* CurrentHandler)
{
	if (CurrentHandler != this) return;

	UGameplayStatics::SetGamePaused(this, true);
	Show();
}

void UDialogueWidgetBase::OnPathEndReached_Implementation(const FString& PathName, UObject* CurrentHandler)
{
	if (CurrentHandler != this) return;

	Hide();
	UGameplayStatics::SetGamePaused(this, false);
}

void UDialogueWidgetBase::OnBeginStory_Implementation(UInkpotStory* Story)
{
	CachedStory = Story;

	CachedStory->OnContinue().AddDynamic(this, &UDialogueWidgetBase::OnContinue);
	CachedStory->OnMakeChoice().AddDynamic(this, &UDialogueWidgetBase::OnMakeChoice);
}

void UDialogueWidgetBase::OnContinue_Implementation(UInkpotStory* Story)
{
	Text_Cursor->SetVisibility(ESlateVisibility::Collapsed);
	UpdateTextWidget(Story);


	if (Story->GetCurrentText().IsEmpty())
	{
		UInkStorySubsystem* InkStory = GEngine->GetEngineSubsystem<UInkStorySubsystem>();
		if (!InkStory) return;

		if (InkStory->GetCurrentPath() != "INVALID") InkStory->ContinueStory();
	}
}

void UDialogueWidgetBase::OnMakeChoice_Implementation(UInkpotStory* Story, UInkpotChoice* Choice)
{
	LineProgress = 0;
	bIsShowingLine = false;

	UpdateChoicesView(Story);
}

void UDialogueWidgetBase::OnEntryGenerated_Implementation(UUserWidget* Widget)
{
	auto& EntryWidgets = LV_Choices->GetDisplayedEntryWidgets();
	int32 Index = EntryWidgets.Find(Widget);

	IStoryChoiceInterface::Execute_Reset(Widget);
	IStoryChoiceInterface::Execute_SetText(Widget, CachedStory->GetCurrentChoices()[Index]->GetText());
	IStoryChoiceInterface::Execute_SetChoiceIndex(Widget, Index);
	IStoryChoiceInterface::Execute_SetChoiceParent(Widget, LV_Choices.Get());
}

void UDialogueWidgetBase::UpdateTextWidget_Implementation(UInkpotStory* Story)
{
	UInkpotLine* CurrentStoryLine = Story->GetCurrentLine();
	auto& Tags = CurrentStoryLine->GetTags();

	for (const auto& Tag : Tags)
	{
		HandleTag(Tag);
	}

	ShowLine(CurrentStoryLine->GetText());
}

bool UDialogueWidgetBase::UpdateChoicesView_Implementation(UInkpotStory* Story)
{
	if (Story->HasChoices())
	{
		LV_Choices->SetListItems(Story->GetCurrentChoices());
		LV_Choices->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
		return true;
	} else
	{
		LV_Choices->SetVisibility(ESlateVisibility::Collapsed);
		LV_Choices->ClearListItems();
		return false;
	}
}
