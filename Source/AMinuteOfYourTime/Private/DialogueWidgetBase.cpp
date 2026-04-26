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
	if (!bPersistentLog)
	{
		CachedSegmentText.Empty();
	}
	else if (!CachedSegmentText.IsEmpty())
	{
		CachedSegmentText += TEXT("\n");
	}

	if (CurrentLine.IsEmpty())
	{
		if (IsValid(LineText) && !bPersistentLog)
		{
			LineText->SetText(FText::GetEmpty());
		}
		
		bHasFinishedPlaying = true;
		OnLineFinishedPlaying();

		LineText->SetVisibility(ESlateVisibility::Hidden);
	}
	else
	{
		bHasFinishedPlaying = false;
		
		TimerManager.SetTimer(LetterTimer, this, &ThisClass::PlayNextLetter, LetterPlayTime, true);

		LineText->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
	}
}

void UDialogueWidgetBase::Reset()
{
	GetWorld()->GetTimerManager().SetTimer(LetterTimer, [&]
	{
		Segments.Empty();
		if (!bPersistentLog)
		{
			CachedSegmentText.Empty();
		}
		else if (!CachedSegmentText.IsEmpty())
		{
			CachedSegmentText += TEXT("\n");
		}

		if (IsValid(LineText))
		{
			LineText->SetText(FText::GetEmpty());
		}
			
		CalculateWrappedString();

		FString WrappedString = CalculateSegments();

		if (IsValid(LineText))
		{
			LineText->SetText(FText::FromString(WrappedString));
		}

		bHasFinishedPlaying = true;

		LineText->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
	} , 0.00001f, false);
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
		UInkStorySubsystem* InkStory = GetGameInstance()->GetSubsystem<UInkStorySubsystem>();
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
		if (IsValid(LineText))
		{
			LineText->SetText(FText::GetEmpty());
		}
		
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

        const FVector2D TextBoxSize = LineText->GetCachedGeometry().GetLocalSize();
    	
        Layout->SetWrappingWidth(TextBoxSize.X);
    	if (LineText->JustifyDecorator)
    		LineText->JustifyDecorator->SetMeasurePassOnly(true);

    	// When using justification tags inline with non-justified text, it will start a new run for the justified
    	// text, but it will try to put this new run on the same line as the non-justified text depending on whether
    	// there's space for it. So we insert newline characters ourselves to ensure inlined justified text always
    	// gets put on a new line, and any non-justified text after it also goes onto a new line.
    	FString ProcessedLine = CurrentLine.ToString();
    	/*UE_LOG(LogTemp, Warning, TEXT("In Line: %s, Current Text: %s, Cached Text: %s"),
    		*CurrentLine.ToString(), *LineText->GetText().ToString(), *CachedSegmentText);*/
    	// Step 1: Replace <br> inside justify tags by splitting them into multiple justify tags
		for (const FString& Tag : TArray<FString>{ TEXT("Centered"), TEXT("Right"), TEXT("Left"), TEXT("Justified") })
		{
		    FString OpenTag = FString::Printf(TEXT("<%s>"), *Tag);
		    FString CloseAndReopen = FString::Printf(TEXT("</>\n<%s>"), *Tag);

		    int32 OpenIdx = ProcessedLine.Find(OpenTag);
		    while (OpenIdx != INDEX_NONE)
		    {
		        int32 ContentStart = OpenIdx + OpenTag.Len();
		        int32 CloseIdx = ProcessedLine.Find(TEXT("</>"), ESearchCase::IgnoreCase, ESearchDir::FromEnd);
		        if (CloseIdx != INDEX_NONE)
		        {
		            FString Content = ProcessedLine.Mid(ContentStart, CloseIdx - ContentStart);
		            FString NewContent = Content.Replace(TEXT("<br>"), *CloseAndReopen);
		            if (Content != NewContent)
		            {
		                ProcessedLine = ProcessedLine.Left(ContentStart) + NewContent + ProcessedLine.Mid(CloseIdx);
		            }
		        }
		        OpenIdx = ProcessedLine.Find(OpenTag, ESearchCase::IgnoreCase, ESearchDir::FromStart,
		            OpenIdx + OpenTag.Len());
		    }
		}

		// Step 2: Replace remaining <br> outside justify tags with \n
		ProcessedLine = ProcessedLine.Replace(TEXT("<br>"), TEXT("\n"));

		// Step 3: Insert \n before and after justify tags so they're on their own line models
		for (const FString& Tag : TArray<FString>{ TEXT("Centered"), TEXT("Right"), TEXT("Left"), TEXT("Justified") })
		{
		    FString OpenTag = FString::Printf(TEXT("<%s>"), *Tag);
		    int32 OpenIdx = ProcessedLine.Find(OpenTag);
		    while (OpenIdx != INDEX_NONE)
		    {
		        // Insert \n before opening tag if not already there and not at start
		        if (OpenIdx > 0 && ProcessedLine[OpenIdx - 1] != '\n')
		        {
		            ProcessedLine.InsertAt(OpenIdx, TEXT("\n"));
		            OpenIdx += 1;
		        }

		        // Find the matching closing tag
		        int32 CloseIdx = ProcessedLine.Find(TEXT("</>"), ESearchCase::IgnoreCase, ESearchDir::FromStart, OpenIdx + OpenTag.Len());
		        if (CloseIdx != INDEX_NONE)
		        {
		            int32 InsertPos = CloseIdx + 3;
		            if (InsertPos >= ProcessedLine.Len() || ProcessedLine[InsertPos] != '\n')
		            {
		                ProcessedLine.InsertAt(InsertPos, TEXT("\n"));
		            }
		        }

		        OpenIdx = ProcessedLine.Find(OpenTag, ESearchCase::IgnoreCase, ESearchDir::FromStart,
		            OpenIdx + OpenTag.Len());
		    }
		}

		//UE_LOG(LogTemp, Warning, TEXT("Processed line is %s"), *ProcessedLine);

		Layout->ClearLines();
		Marshaller->SetText(ProcessedLine, *Layout.Get());
        Layout->UpdateIfNeeded();
    	if (LineText->JustifyDecorator)
    		LineText->JustifyDecorator->SetMeasurePassOnly(false);

        // We iterate LineViews→Blocks to get visual wrapping order, but we
        // need to merge all blocks that share the same underlying IRun pointer
        // so that a multi-word decorated run isn't split into N segments.
        //
        // Strategy: accumulate blocks per run. When the run changes (or we hit
        // a line-view boundary we want to preserve as \n), flush the accumulator.

        // First pass: collect (lineViewIndex, block) pairs in order.
    	bool bHasWrittenText = false;
    	int32 LastLineViewIndex = 0;

    	int32 i = 0;
    	const TArray<FTextLayout::FLineView>& LineViews = Layout->GetLineViews();

    	// Flatten all blocks with their line view index
    	struct FBlockEntry
    	{
    		int32 LineViewIndex;
    		TSharedRef<ILayoutBlock> Block;
    	};
    	TArray<FBlockEntry> AllBlocks;
    	for (int32 ViewIdx = 0; ViewIdx < LineViews.Num(); ++ViewIdx)
    	{
    		for (TSharedRef<ILayoutBlock> Block : LineViews[ViewIdx].Blocks)
    		{
    			AllBlocks.Add({ ViewIdx, Block });
    		}
    	}

    	for (int32 DebugIdx = 0; DebugIdx < AllBlocks.Num(); ++DebugIdx)
    	{
    		FString DebugText;
    		AllBlocks[DebugIdx].Block->GetRun()->AppendTextTo(DebugText, AllBlocks[DebugIdx].Block->GetTextRange());
    		UE_LOG(LogTemp, Warning, TEXT("Block %d: Run='%s', LineView=%d, Text='%s'"), 
				DebugIdx,
				*AllBlocks[DebugIdx].Block->GetRun()->GetRunInfo().Name,
				AllBlocks[DebugIdx].LineViewIndex,
				*DebugText);
    	}

    	while (i < AllBlocks.Num())
    	{
    		const FBlockEntry& First = AllBlocks[i];
    		TSharedRef<IRun> Run = First.Block->GetRun();
    		const int32 ThisLineView = First.LineViewIndex;
    		//UE_LOG(LogTemp, Warning, TEXT("Run for block %i is %s"), i, *Run->GetRunInfo().Name);

    		// Merge consecutive blocks that share BOTH the same run AND the same line view
    		FTextRange MergedRange = First.Block->GetTextRange();
    		int32 j = i + 1;
    		while (j < AllBlocks.Num()
				&& AllBlocks[j].Block->GetRun() == Run
				&& AllBlocks[j].LineViewIndex == ThisLineView)
    		{
    			MergedRange.EndIndex = AllBlocks[j].Block->GetTextRange().EndIndex;
    			++j;
    		}
    		//UE_LOG(LogTemp, Warning, TEXT("Merged range is %i to %i"), MergedRange.BeginIndex, MergedRange.EndIndex);

    		// Emit \n if we've moved to a new line view
    		if (bHasWrittenText && ThisLineView != LastLineViewIndex)
    		{
    			Segments.Add(FDialogueTextSegment{ TEXT("\n") });
    			++MaxLetterIndex;
    		}
    		LastLineViewIndex = ThisLineView;

    		FString MergedText;
    		Run->AppendTextTo(MergedText, MergedRange);

    		if (MergedText.Len() == 1 && MergedText[0] == 0x200B)
    		{
    			MergedText.Empty();
    		}

    		FDialogueTextSegment Segment;
    		Segment.Text = MergedText;
    		Segment.RunInfo = Run->GetRunInfo();
    		UE_LOG(LogTemp, Warning, TEXT("Segment text: '%s', Run: '%s', LineView: %d"), 
				*MergedText, *Run->GetRunInfo().Name, ThisLineView);
    		Segments.Add(Segment);

    		MaxLetterIndex += FMath::Max(
				Segment.Text.Len(),
				Segment.RunInfo.Name.IsEmpty() ? 0 : 1);

    		if (!Segment.Text.IsEmpty() || !Segment.RunInfo.Name.IsEmpty())
    		{
    			bHasWrittenText = true;
    		}

    		i = j;
    	}

        Layout->SetWrappingWidth(0);
    }
    else
    {
        Segments.Add(FDialogueTextSegment{ CurrentLine.ToString() });
        MaxLetterIndex = Segments[0].Text.Len();
    }
}

FString UDialogueWidgetBase::CalculateSegments()
{
    static const TArray<FString> JustificationTags = {
        TEXT("Centered"), TEXT("Right"), TEXT("Left"), TEXT("Justified")
    };

    FString Result = CachedSegmentText;

    int32 Idx = CachedLetterIndex;
    while (Idx <= CurrentLetterIndex && CurrentSegmentIndex < Segments.Num())
    {
        const FDialogueTextSegment& Segment = Segments[CurrentSegmentIndex];

        const bool bIsJustifyTag = JustificationTags.Contains(Segment.RunInfo.Name);

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
                ++Idx;
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

        	if (bIsJustifyTag)
        	{
        		for (int i = 0; i < Segment.Text.Len() - LettersLeft; ++i)
        		{
        			// Insert a bunch of spaces so that justified text writes
        			// from its leftmost edge (still a bit jank, I think due to
        			// characters having variable width)
        			Result += " ";
        		}
        	}

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

	if (Story->GetCurrentText().IsEmpty() && InkStory->GetCurrentPath() != "INVALID")
		InkStory->ContinueStory();
}

void UDialogueWidgetBase::OnMakeChoice_Implementation(UInkpotStory* Story, UInkpotChoice* Choice)
{
	UInkStorySubsystem* InkStory = GetGameInstance()->GetSubsystem<UInkStorySubsystem>();
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
