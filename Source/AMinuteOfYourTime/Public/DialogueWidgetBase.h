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

UCLASS(Abstract, BlueprintType, Blueprintable, meta = (DisplayName = "StoryWidget"))
class AMINUTEOFYOURTIME_API UDialogueWidgetBase : public UUserWidget
{
	GENERATED_BODY()

public:
	UDialogueWidgetBase(const FObjectInitializer& ObjectInitializer);

	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable)
	void Show();
	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable)
	void Hide();

	UFUNCTION(BlueprintCallable, Category = "Dialogue Box")
	void PlayLine(const FText& InLine);

	UFUNCTION(BlueprintCallable, Category = "Dialogue Box")
	void Reset();

	UFUNCTION(BlueprintCallable, Category = "Dialogue Box")
	void GetCurrentLine(FText& OutLine) const { LineText->GetCurrentLine(OutLine); }

	UFUNCTION(BlueprintCallable, Category = "Dialogue Box")
	bool HasFinishedPlayingLine() const { return LineText->HasFinishedPlayingLine(); }

	UFUNCTION(BlueprintCallable, Category = "Dialogue Box")
	void SkipToLineEnd();

protected:
	virtual void NativeConstruct() override;

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

	UPROPERTY()
	FText CurrentLine;

	UPROPERTY(BlueprintReadWrite)
	bool bAllowSkip = true;

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

	UPROPERTY(BlueprintReadWrite, meta = (BindWidgetOptional))
	TObjectPtr<USizeBox> SB_DialogueRoot;

	UPROPERTY(BlueprintReadWrite, meta = (BindWidgetOptional))
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
