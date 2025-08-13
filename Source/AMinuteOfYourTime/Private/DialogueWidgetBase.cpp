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
		if (!InkStory) return FReply::Handled();

		if (bIsShowingLine)
		{
			OnSkipLine();
			LineProgress = CurrentLine.ToString().Len();
			//Text_Dialogue->SetText(CurrentLine);
			bIsShowingLine = false;
			Text_Cursor->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
			UpdateChoicesView(InkStory->GetStory());

			return FReply::Handled();
		} else
		{

			if (InkStory->ContinueStory()) Text_Cursor->SetVisibility(ESlateVisibility::Collapsed);
			//UpdateChoicesView(InkStory->GetStory());

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
		OnSkipLine();
		LineProgress = CurrentLine.ToString().Len();
		//Text_Dialogue->SetText(CurrentLine);
		bIsShowingLine = false;

		UInkStorySubsystem* InkStory = GEngine->GetEngineSubsystem<UInkStorySubsystem>();
		if (!InkStory) return FReply::Handled();

		if (InkStory->ContinueStory()) Text_Cursor->SetVisibility(ESlateVisibility::Collapsed);
		//UpdateChoicesView(InkStory->GetStory());

		return FReply::Handled();
	}

	return FReply::Unhandled();
}

void UDialogueWidgetBase::OnPathStarted_Implementation(const FString& PathName)
{
	UGameplayStatics::SetGamePaused(this, true);
	Show();
}

void UDialogueWidgetBase::OnPathEndReached_Implementation(const FString& PathName)
{
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
	//UpdateChoicesView(Story);
}

void UDialogueWidgetBase::OnMakeChoice_Implementation(UInkpotStory* Story, UInkpotChoice* Choice)
{
	LineProgress = 0;
	bIsShowingLine = false;

	UInkStorySubsystem* InkStory = GEngine->GetEngineSubsystem<UInkStorySubsystem>();
	if (!InkStory) return;

	InkStory->ContinueStory();
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

void UDialogueWidgetBase::OnChoiceClicked_Implementation(int32 ChoiceIndex)
{
	UInkStorySubsystem* InkStory = GEngine->GetEngineSubsystem<UInkStorySubsystem>();
	if (!InkStory) return;

	InkStory->MakeChoiceIndex(ChoiceIndex);
}

void UDialogueWidgetBase::UpdateTextWidget_Implementation(UInkpotStory* Story)
{
	UInkpotLine* CurrentStoryLine = Story->GetCurrentLine();
	auto& Tags = CurrentStoryLine->GetTags();

	if (!Tags.IsEmpty())
	{
		Text_Speaker->SetText(FText::FromString(Tags[0]));
		SB_SpeakerRoot->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
	} else
	{
		SB_SpeakerRoot->SetVisibility(ESlateVisibility::Collapsed);
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
