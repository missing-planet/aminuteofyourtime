// Copyright (c) 2025, Missing Planet LLC. All rights reserved.


#include "DialogueWidgetBase.h"

#include "InkStorySubsystem.h"
#include "StoryChoiceNativeBase.h"
#include "Inkpot/Inkpot.h"
#include "Kismet/GameplayStatics.h"

UDialogueWidgetBase::UDialogueWidgetBase(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	
}

void UDialogueWidgetBase::PlayLine(const FText& InLine)
{
	LineText->PlayLine(InLine);
}

void UDialogueWidgetBase::Reset()
{
	LineText->Reset();
}

void UDialogueWidgetBase::SkipToLineEnd()
{
	LineText->SkipToLineEnd();

	UpdateChoicesView(CachedStory);
}

void UDialogueWidgetBase::NativeConstruct()
{
	Super::NativeConstruct();

	Hide();

	LineText->OnLineFinishedPlayingEvent.AddDynamic(this, &ThisClass::OnLineFinishedPlaying);
	LineText->OnPlayLetterEvent.AddDynamic(this, &ThisClass::OnPlayLetter);

	Text_Cursor->SetVisibility(ESlateVisibility::Collapsed);
	if (CursorAnimation) PlayAnimation(CursorAnimation, 0, 0);

	UInkpot* InkpotSystem = GEngine->GetEngineSubsystem<UInkpot>();
	UInkStorySubsystem* InkStory = GetGameInstance()->GetSubsystem<UInkStorySubsystem>();
	if (!InkpotSystem || !InkStory) return;

	if (InkStory->HasStoryBegun() && !CachedStory)
	{
		OnBeginStory(InkStory->GetStory());
	} else
	{
		InkpotSystem->EventOnStoryBegin.AddDynamic(this, &UDialogueWidgetBase::OnBeginStory);
	}

	InkStory->PathStartedEvent.AddDynamic(this, &UDialogueWidgetBase::OnPathStarted);
	InkStory->PathEndReachedEvent.AddDynamic(this, &UDialogueWidgetBase::OnPathEndReached);
}

FReply UDialogueWidgetBase::NativeOnMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
	if (InMouseEvent.GetEffectingButton() == FKey("LeftMouseButton"))
	{
		UInkStorySubsystem* InkStory = GetGameInstance()->GetSubsystem<UInkStorySubsystem>();
		if (!InkStory || !bAllowSkip) return FReply::Unhandled();

		if (!LineText->HasFinishedPlayingLine())
		{
			OnSkipLine();
			SkipToLineEnd();
			Text_Cursor->SetVisibility(ESlateVisibility::SelfHitTestInvisible);

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
	return FReply::Handled();
}

void UDialogueWidgetBase::OnPathStarted_Implementation(const FString& PathName, UObject* CurrentHandler)
{
	if (CurrentHandler != this) return;

	//UGameplayStatics::SetGamePaused(this, true);
	Show();
}

void UDialogueWidgetBase::OnPathEndReached_Implementation(const FString& PathName, UObject* CurrentHandler)
{
	if (CurrentHandler != this) return;

	Hide();
	UInkStorySubsystem* InkStory = GetGameInstance()->GetSubsystem<UInkStorySubsystem>();
	if (!InkStory) return;

	InkStory->EventEndReachedEvent.Broadcast(InkStory->GetCurrentPath(), InkStory->GetCurrentStoryHandler());
}

void UDialogueWidgetBase::OnBeginStory_Implementation(UInkpotStory* Story)
{
	CachedStory = Story;

	CachedStory->OnContinue().AddDynamic(this, &UDialogueWidgetBase::OnContinue);
	CachedStory->OnMakeChoice().AddDynamic(this, &UDialogueWidgetBase::OnMakeChoice);
}

void UDialogueWidgetBase::OnContinue_Implementation(UInkpotStory* Story)
{
	UInkStorySubsystem* InkStory = GetGameInstance()->GetSubsystem<UInkStorySubsystem>();
	if (!InkStory || InkStory->GetCurrentStoryHandler() != this) return;

	/*UE_LOG(LogTemp, Warning, TEXT("Handler is: %s, This is: %s"),
		*InkStory->GetCurrentStoryHandler()->GetName(), *GetName());*/

	if (Text_Cursor) Text_Cursor->SetVisibility(ESlateVisibility::Collapsed);
	UpdateTextWidget(Story);
	UpdateChoicesView(Story);

	if (Story->GetCurrentText().IsEmpty() && InkStory->GetCurrentPath() != "INVALID")
		InkStory->ContinueStory();
}

void UDialogueWidgetBase::OnMakeChoice_Implementation(UInkpotStory* Story, UInkpotChoice* Choice)
{
	UInkStorySubsystem* InkStory = GetGameInstance()->GetSubsystem<UInkStorySubsystem>();
	if (!InkStory || InkStory->GetCurrentStoryHandler() != this) return;

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

	FText Line = CurrentStoryLine->GetText();
	
	PlayLine(Line);
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
