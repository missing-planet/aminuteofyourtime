// Copyright (c) 2025, Missing Planet LLC. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "CustomImageDecorator.h"
#include "JustifyTextDecorator.h"
#include "Blueprint/UserWidget.h"
#include "Components/RichTextBlock.h"
#include "Framework/Text/RichTextLayoutMarshaller.h"
#include "Framework/Text/SlateTextLayout.h"
#include "DialogueTextBlock.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnPlayLetterSignature, int32, LetterIndex, const FString&, Letter);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnLineFinishedPlayingSignature);

struct FDialogueTextSegment
{
	FString Text;
	FRunInfo RunInfo;
};

/**
 * 
 */
UCLASS()
class AMINUTEOFYOURTIME_API UDialogueTextBlock : public URichTextBlock
{
	GENERATED_BODY()
	
public:
	
	FORCEINLINE TSharedPtr<FSlateTextLayout> GetTextLayout() const
	{
		return TextLayout;
	}

	FORCEINLINE TSharedPtr<FRichTextLayoutMarshaller> GetTextMarshaller() const
	{
		return TextMarshaller;
	}

	/*UFUNCTION(BlueprintCallable)
	void TickDecorators(float DeltaTime);*/

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FName OverrideStyleRowName;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bOverrideStyle;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bPersistentLog = false;

	UPROPERTY()
	TObjectPtr<UJustifyTextDecorator> JustifyDecorator;

	// The amount of time between printing individual letters (for the "typewriter" effect).
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float LetterPlayTime = 0.025f;

	// The amount of time to wait after finishing the line before actually marking it completed.
	// This helps prevent accidentally progressing dialogue on short lines.
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float EndHoldTime = 0.15f;

	UFUNCTION(BlueprintCallable, Category = "Dialogue Box")
	void PlayLine(const FText& InLine);

	UFUNCTION(BlueprintCallable, Category = "Dialogue Box")
	void SetLine(const FText& InLine);

	UFUNCTION(BlueprintCallable, Category = "Dialogue Box")
	void Reset();

	UFUNCTION(BlueprintCallable, Category = "Dialogue Box")
	void GetCurrentLine(FText& OutLine) const { OutLine = CurrentLine; }

	UFUNCTION(BlueprintCallable, Category = "Dialogue Box")
	bool HasFinishedPlayingLine() const { return bHasFinishedPlaying; }

	UFUNCTION(BlueprintCallable, Category = "Dialogue Box")
	void SkipToLineEnd();

	UPROPERTY(BlueprintAssignable)
	FOnPlayLetterSignature OnPlayLetterEvent;

	UPROPERTY(BlueprintAssignable)
	FOnLineFinishedPlayingSignature OnLineFinishedPlayingEvent;

protected:
	virtual TSharedRef<SWidget> RebuildWidget() override;

private:
	void PlayNextLetter();

	void CalculateWrappedString();
	FString CalculateSegments();

private:
	TSharedPtr<FSlateTextLayout> TextLayout;
	TSharedPtr<FRichTextLayoutMarshaller> TextMarshaller;

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
};
