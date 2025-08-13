// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "CustomTextNativeBase.h"
#include "Blueprint/UserWidget.h"
#include "Components/ListView.h"
#include "Components/Overlay.h"
#include "Components/SizeBox.h"
#include "Inkpot/InkpotStory.h"
#include "DialogueWidgetBase.generated.h"

UCLASS(Abstract, BlueprintType, Blueprintable)
class AMINUTEOFYOURTIME_API UDialogueWidgetBase : public UUserWidget
{
	GENERATED_BODY()

public:

	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable)
	void Show();
	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable)
	void Hide();

protected:
	virtual void NativePreConstruct() override;
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
	void OnChoiceClicked(int32 ChoiceIndex);

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	void UpdateTextWidget(UInkpotStory* Story);
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	bool UpdateChoicesView(UInkpotStory* Story);

	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable)
	void ShowLine(const FText& Line);

	UFUNCTION(BlueprintImplementableEvent)
	void OnSkipLine();

protected:

	UPROPERTY(BlueprintReadWrite)
	float DisplayTime = 0.1f;
	UPROPERTY(BlueprintReadWrite)
	FText CurrentLine;
	UPROPERTY(BlueprintReadWrite)
	int32 LineProgress = 0;
	UPROPERTY(BlueprintReadWrite)
	bool bIsShowingLine = false;
	UPROPERTY(BlueprintReadWrite)
	bool bAllowSkip = true;

	UPROPERTY(BlueprintReadWrite, Transient, meta = (BindWidgetAnimOptional))
	TObjectPtr<UWidgetAnimation> CursorAnimation;

	UPROPERTY(BlueprintReadWrite, meta = (BindWidgetOptional))
	TObjectPtr<UCustomTextNativeBase> Text_Speaker;

	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	TObjectPtr<UCustomTextNativeBase> Text_Dialogue;

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
	void OnChoiceClicked_Implementation(int32 ChoiceIndex);

	void UpdateTextWidget_Implementation(UInkpotStory* Story);
	bool UpdateChoicesView_Implementation(UInkpotStory* Story);
};
