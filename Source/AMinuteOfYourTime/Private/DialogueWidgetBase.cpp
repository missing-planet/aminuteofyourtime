// Copyright (c) 2025, Missing Planet LLC. All rights reserved.


#include "DialogueWidgetBase.h"

#include "InkStorySubsystem.h"
#include "StoryChoiceNativeBase.h"
#include "Inkpot/Inkpot.h"
#include "Kismet/GameplayStatics.h"

UDialogueWidgetBase::UDialogueWidgetBase(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	bHasFinishedPlaying = true;
}

void UDialogueWidgetBase::PlayLine(const FText& InLine)
{
	check(GetWorld());

	FTimerManager& TimerManager = GetWorld()->GetTimerManager();
	TimerManager.ClearTimer(LetterTimer);

	CurrentLine = InLine;
	CurrentLetterIndex = 0;
	CachedLetterIndex = 0;
	CurrentSegmentIndex = 0;
	MaxLetterIndex = 0;
	Segments.Empty();
	if (!bPersistentLog) CachedSegmentText.Empty();

	if (CurrentLine.IsEmpty())
	{
		if (IsValid(LineText))
		{
			LineText->SetText(FText::GetEmpty());
		}

		bHasFinishedPlaying = true;
		OnLineFinishedPlaying();

		LineText->SetVisibility(ESlateVisibility::Hidden);
	}
	else
	{
		if (IsValid(LineText))
		{
			LineText->SetText(FText::GetEmpty());
		}

		bHasFinishedPlaying = false;

		TimerManager.SetTimer(LetterTimer, this, &ThisClass::PlayNextLetter, LetterPlayTime, true);

		LineText->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
	}
}

void UDialogueWidgetBase::SkipToLineEnd()
{
	FTimerManager& TimerManager = GetWorld()->GetTimerManager();
	TimerManager.ClearTimer(LetterTimer);

	CurrentLetterIndex = MaxLetterIndex - 1;
	if (IsValid(LineText))
	{
		LineText->SetText(FText::FromString(CalculateSegments()));
	}

	bHasFinishedPlaying = true;
	OnLineFinishedPlaying();

	UpdateChoicesView(CachedStory);
}

void UDialogueWidgetBase::NativeConstruct()
{
	Super::NativeConstruct();

	Hide();

	Text_Cursor->SetVisibility(ESlateVisibility::Collapsed);
	if (CursorAnimation) PlayAnimation(CursorAnimation, 0, 0);

	UInkpot* InkpotSystem = GEngine->GetEngineSubsystem<UInkpot>();
	UInkStorySubsystem* InkStory = GEngine->GetEngineSubsystem<UInkStorySubsystem>();
	if (!InkpotSystem || !InkStory) return;

	InkpotSystem->EventOnStoryBegin.AddDynamic(this, &UDialogueWidgetBase::OnBeginStory);

	InkStory->PathStartedEvent.AddDynamic(this, &UDialogueWidgetBase::OnPathStarted);
	InkStory->PathEndReachedEvent.AddDynamic(this, &UDialogueWidgetBase::OnPathEndReached);
}

void UDialogueWidgetBase::NativeDestruct()
{
	Super::NativeDestruct();

	check(GetWorld());

	FTimerManager& TimerManager = GetWorld()->GetTimerManager();
	TimerManager.ClearTimer(LetterTimer);
}

FReply UDialogueWidgetBase::NativeOnMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
	if (InMouseEvent.GetEffectingButton() == FKey("LeftMouseButton"))
	{
		UInkStorySubsystem* InkStory = GEngine->GetEngineSubsystem<UInkStorySubsystem>();
		if (!InkStory || !bAllowSkip) return FReply::Unhandled();

		if (!bHasFinishedPlaying)
		{
			OnSkipLine();
			SkipToLineEnd();
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
	return FReply::Handled();
}

void UDialogueWidgetBase::PlayNextLetter()
{
	if (Segments.IsEmpty())
	{
		CalculateWrappedString();
	}

	FString WrappedString = CalculateSegments();

	// TODO: How do we keep indexing of text i18n-friendly?
	if (CurrentLetterIndex < MaxLetterIndex)
	{
		if (IsValid(LineText))
		{
			LineText->SetText(FText::FromString(WrappedString));
		}

		OnPlayLetter(CurrentLetterIndex, LineText->GetText().ToString().Right(1));
		++CurrentLetterIndex;
	}
	else
	{
		if (IsValid(LineText))
		{
			LineText->SetText(FText::FromString(CalculateSegments()));
		}

		FTimerManager& TimerManager = GetWorld()->GetTimerManager();
		TimerManager.ClearTimer(LetterTimer);

		TimerManager.SetTimer(LetterTimer, this, &UDialogueWidgetBase::SkipToLineEnd, EndHoldTime, false);
	}
}

void UDialogueWidgetBase::CalculateWrappedString()
{
	if (IsValid(LineText) && LineText->GetTextLayout().IsValid())
	{
		TSharedPtr<FSlateTextLayout> Layout = LineText->GetTextLayout();
		TSharedPtr<FRichTextLayoutMarshaller> Marshaller = LineText->GetTextMarshaller();

		const FGeometry& TextBoxGeometry = LineText->GetCachedGeometry();
		const FVector2D TextBoxSize = TextBoxGeometry.GetLocalSize();

		Layout->SetWrappingWidth(TextBoxSize.X);
		Marshaller->SetText(CurrentLine.ToString(), *Layout.Get());
		Layout->UpdateIfNeeded();

		bool bHasWrittenText = false;
		for (const FTextLayout::FLineView& View : Layout->GetLineViews())
		{
			const FTextLayout::FLineModel& Model = Layout->GetLineModels()[View.ModelIndex];

			for (TSharedRef<ILayoutBlock> Block : View.Blocks)
			{
				TSharedRef<IRun> Run = Block->GetRun();

				FDialogueTextSegment Segment;
				Run->AppendTextTo(Segment.Text, Block->GetTextRange());

				// HACK: For some reason image decorators (and possibly other decorators that don't
				// have actual text inside them) result in the run containing a zero width space instead of
				// nothing. This messes up our checks for whether the text is empty or not, which doesn't
				// have an effect on image decorators but might cause issues for other custom ones.
				if (Segment.Text.Len() == 1 && Segment.Text[0] == 0x200B)
				{
					Segment.Text.Empty();
				}

				Segment.RunInfo = Run->GetRunInfo();
				Segments.Add(Segment);

				// A segment with a named run should still take up time for the typewriter effect.
				MaxLetterIndex += FMath::Max(Segment.Text.Len(), Segment.RunInfo.Name.IsEmpty() ? 0 : 1);

				if (!Segment.Text.IsEmpty() || !Segment.RunInfo.Name.IsEmpty())
				{
					bHasWrittenText = true;
				}
			}

			if (bHasWrittenText)
			{
				Segments.Add(FDialogueTextSegment{ TEXT("\n") });
				++MaxLetterIndex;
			}
		}

		Layout->SetWrappingWidth(0);
		LineText->SetText(LineText->GetText());
	}
	else
	{
		Segments.Add(FDialogueTextSegment{ CurrentLine.ToString() });
		MaxLetterIndex = Segments[0].Text.Len();
	}
}

FString UDialogueWidgetBase::CalculateSegments()
{
	FString Result = CachedSegmentText;

	int32 Idx = CachedLetterIndex;
	while (Idx <= CurrentLetterIndex && CurrentSegmentIndex < Segments.Num())
	{
		const FDialogueTextSegment& Segment = Segments[CurrentSegmentIndex];
		if (!Segment.RunInfo.Name.IsEmpty())
		{
			Result += FString::Printf(TEXT("<%s"), *Segment.RunInfo.Name);

			if (!Segment.RunInfo.MetaData.IsEmpty())
			{
				for (const TTuple<FString, FString>& MetaData : Segment.RunInfo.MetaData)
				{
					Result += FString::Printf(TEXT(" %s=\"%s\""), *MetaData.Key, *MetaData.Value);
				}
			}

			if (Segment.Text.IsEmpty())
			{
				Result += TEXT("/>");
				++Idx; // This still takes up an index for the typewriter effect.
			}
			else
			{
				Result += TEXT(">");
			}
		}

		bool bIsSegmentComplete = true;
		if (!Segment.Text.IsEmpty())
		{
			int32 LettersLeft = CurrentLetterIndex - Idx + 1;
			bIsSegmentComplete = LettersLeft >= Segment.Text.Len();
			LettersLeft = FMath::Min(LettersLeft, Segment.Text.Len());
			Idx += LettersLeft;

			Result += Segment.Text.Mid(0, LettersLeft);

			if (!Segment.RunInfo.Name.IsEmpty())
			{
				Result += TEXT("</>");
			}
		}

		if (bIsSegmentComplete)
		{
			CachedLetterIndex = Idx;
			CachedSegmentText = Result;
			++CurrentSegmentIndex;
		}
		else
		{
			break;
		}
	}

	return Result;
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
	UInkStorySubsystem* InkStory = GEngine->GetEngineSubsystem<UInkStorySubsystem>();
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
	UInkStorySubsystem* InkStory = GEngine->GetEngineSubsystem<UInkStorySubsystem>();
	if (!InkStory || InkStory->GetCurrentStoryHandler() != this) return;

	if (Text_Cursor) Text_Cursor->SetVisibility(ESlateVisibility::Collapsed);
	UpdateTextWidget(Story);

	if (Story->GetCurrentText().IsEmpty() && InkStory->GetCurrentPath() != "INVALID")
		InkStory->ContinueStory();
}

void UDialogueWidgetBase::OnMakeChoice_Implementation(UInkpotStory* Story, UInkpotChoice* Choice)
{
	UInkStorySubsystem* InkStory = GEngine->GetEngineSubsystem<UInkStorySubsystem>();
	if (!InkStory || InkStory->GetCurrentStoryHandler() != this) return;

	//LineProgress = 0;
	//bIsShowingLine = false;

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

	PlayLine(CurrentStoryLine->GetText());
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
