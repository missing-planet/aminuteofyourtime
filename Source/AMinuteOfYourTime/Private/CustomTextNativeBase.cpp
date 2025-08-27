// Fill out your copyright notice in the Description page of Project Settings.


#include "CustomTextNativeBase.h"

void UCustomTextNativeBase::SetDefaultStyle(const FName& NewDefault)
{
	if (!TextElement) return;

	if (auto* Row = TextElement->GetTextStyleSet()->FindRow<FRichTextStyleRow>(NewDefault, "SetDefaultStyle"))
	{
		TextElement->SetDefaultTextStyle(Row->TextStyle);
	}
}

void UCustomTextNativeBase::SetText_Implementation(const FText& InText)
{
	if (!TextElement) return;

	TextElement->SetText(InText);
}

void UCustomTextNativeBase::SetJustification_Implementation(ETextJustify::Type Justification)
{
	if (!TextElement) return;

	TextElement->SetJustification(Justification);
}

void UCustomTextNativeBase::SetAutoWrap_Implementation(bool bAutoWrap)
{
	if (!TextElement) return;

	TextElement->SetAutoWrapText(bAutoWrap);
}
