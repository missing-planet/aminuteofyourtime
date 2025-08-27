// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Components/RichTextBlock.h"
#include "CustomTextNativeBase.generated.h"

/**
 * 
 */
UCLASS(BlueprintType, MinimalAPI)
class UCustomTextNativeBase : public UUserWidget
{
	GENERATED_BODY()

public:

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	void SetText(const FText& InText);

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	void SetJustification(ETextJustify::Type Justification);

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	void SetAutoWrap(bool bAutoWrap);

	UFUNCTION(BlueprintCallable)
	void SetDefaultStyle(const FName& NewDefault);

protected:

	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	TObjectPtr<URichTextBlock> TextElement;

private:

	void SetText_Implementation(const FText& InText);
	void SetJustification_Implementation(ETextJustify::Type Justification);
	void SetAutoWrap_Implementation(bool bAutoWrap);
};
