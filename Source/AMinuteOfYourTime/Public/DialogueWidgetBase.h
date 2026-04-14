// Copyright (c) 2025, Missing Planet LLC. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "CustomTextNativeBase.h"
#include "Blueprint/UserWidget.h"
#include "Components/ListView.h"
#include "Components/Overlay.h"
#include "Components/SizeBox.h"
#include "Inkpot/InkpotStory.h"
#include "DialogueTextBlock.h"
#include "DialogueWidgetBase.generated.h"

UCLASS(Abstract, BlueprintType, Blueprintable)
class AMINUTEOFYOURTIME_API UDialogueWidgetBase : public UUserWidget
{
	GENERATED_BODY()

public:
	UDialogueWidgetBase(const FObjectInitializer& ObjectInitializer);

	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable)
	void Show();
	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable)
	void Hide();

	// The amount of time between printing individual letters (for the "typewriter" effect).
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dialogue Box")
	float LetterPlayTime = 0.025f;

	// The amount of time to wait after finishing the line before actually marking it completed.
	// This helps prevent accidentally progressing dialogue on short lines.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dialogue Box")
	float EndHoldTime = 0.15f;

	UFUNCTION(BlueprintCallable, Category = "Dialogue Box")
	void PlayLine(const FText& InLine);

	UFUNCTION(BlueprintCallable, Category = "Dialogue Box")
	void Reset();

	UFUNCTION(BlueprintCallable, Category = "Dialogue Box")
	void GetCurrentLine(FText& OutLine) const { OutLine = CurrentLine; }

	UFUNCTION(BlueprintCallable, Category = "Dialogue Box")
	bool HasFinishedPlayingLine() const { return bHasFinishedPlaying; }

	UFUNCTION(BlueprintCallable, Category = "Dialogue Box")
	void SkipToLineEnd();

protected:
	virtual void NativeConstruct() override;

	virtual void NativeDestruct() override;

	virtual FReply NativeOnMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent) override;
	virtual FReply
		NativeOnMouseButtonDoubleClick(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent) override;

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	void OnPathStarted(const FString& PathName, UObject* CurrentHandler);
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	void OnPathEndReached(const FString& PathName, UObject* CurrentHandler);

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	void OnBeginStory(UInkpotStory* Story);
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	void OnContinue(UInkpotStory* Story);
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	void OnMakeChoice(UInkpotStory* Story, UInkpotChoice* Choice);

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	void OnEntryGenerated(UUserWidget* Widget);

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	void UpdateTextWidget(UInkpotStory* Story);
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	bool UpdateChoicesView(UInkpotStory* Story);

	UFUNCTION(BlueprintImplementableEvent)
	void OnSkipLine();

	UFUNCTION(BlueprintImplementableEvent)
	void HandleTag(const FString& Tag);

	UFUNCTION(BlueprintImplementableEvent, Category = "Dialogue Box")
	void OnPlayLetter(int32 LetterIndex, const FString& Letter);

	UFUNCTION(BlueprintImplementableEvent, Category = "Dialogue Box")
	void OnLineFinishedPlaying();

protected:
	UFUNCTION()
	void PlayNextLetter();

	void CalculateWrappedString();
	FString CalculateSegments();

	UPROPERTY()
	FText CurrentLine;

	TArray<FDialogueTextSegment> Segments;

	// The section of the text that's already been printed out and won't ever change.
	// This lets us cache some of the work we've already done. We can't cache absolutely
	// everything as the last few characters of a string may change if they're related to
	// a named run that hasn't been completed yet.
	FString CachedSegmentText;
	int32 CachedLetterIndex = 0;

	int32 CurrentSegmentIndex = 0;
	int32 CurrentLetterIndex = 0;
	int32 MaxLetterIndex = 0;

	uint32 bHasFinishedPlaying : 1;

	FTimerHandle LetterTimer;

	UPROPERTY(BlueprintReadWrite)
	bool bAllowSkip = true;
	UPROPERTY(BlueprintReadWrite)
	bool bPersistentLog = false;

	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	TObjectPtr<UDialogueTextBlock> LineText;

	UPROPERTY(BlueprintReadWrite, Transient, meta = (BindWidgetAnimOptional))
	TObjectPtr<UWidgetAnimation> CursorAnimation;

	UPROPERTY(BlueprintReadWrite, meta = (BindWidgetOptional))
	TObjectPtr<UCustomTextNativeBase> Text_Speaker;

	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	TObjectPtr<UWidget> Text_Cursor;

	UPROPERTY(BlueprintReadWrite, meta = (BindWidgetOptional))
	TObjectPtr<USizeBox> SB_SpeakerRoot;

	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	TObjectPtr<USizeBox> SB_DialogueRoot;

	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	TObjectPtr<UOverlay> Overlay_Root;

	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	TObjectPtr<UListView> LV_Choices;

private:

	UPROPERTY()
	TObjectPtr<UInkpotStory> CachedStory;

private:

	void OnPathStarted_Implementation(const FString& PathName, UObject* CurrentHandler);
	void OnPathEndReached_Implementation(const FString& PathName, UObject* CurrentHandler);

	void OnBeginStory_Implementation(UInkpotStory* Story);
	void OnContinue_Implementation(UInkpotStory* Story);
	void OnMakeChoice_Implementation(UInkpotStory* Story, UInkpotChoice* Choice);

	void OnEntryGenerated_Implementation(UUserWidget* Widget);

	void UpdateTextWidget_Implementation(UInkpotStory* Story);
	bool UpdateChoicesView_Implementation(UInkpotStory* Story);
};
