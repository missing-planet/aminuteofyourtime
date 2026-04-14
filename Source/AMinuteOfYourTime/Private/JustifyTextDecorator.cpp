// Copyright (c) 2025, Missing Planet LLC. All rights reserved.


#include "JustifyTextDecorator.h"

#include "DialogueTextBlock.h"
#include "Components/RichTextBlock.h"
#include "Framework/Text/SlateTextRun.h"
#include "Framework/Text/SlateWidgetRun.h"
#include "Widgets/Text/SRichTextBlock.h"

// This only exists to expose this one property without requiring me to edit the original base class
class URichTextBlockExposeDecorators : public URichTextBlock
{
public:
	const TArray<TObjectPtr<URichTextBlockDecorator>>& GetDecorators()
	{
		return InstanceDecorators;
	}

	TSharedPtr<class FSlateStyleSet> GetStyleInstance()
	{
		return StyleInstance;
	}
};

class FJustifyDecorator : public ITextDecorator
{
public:
	FJustifyDecorator(URichTextBlock* InOwner, EHorizontalAlignment InAlignment)
		: Owner(InOwner), Alignment(InAlignment) {}

	virtual bool Supports(const FTextRunParseResults& RunInfo, const FString& Text) const override
	{
		static const TArray<FString> Tags = {
			TEXT("Centered"), TEXT("Right"), TEXT("Left"), TEXT("Justified")
		};
		return Tags.Contains(RunInfo.Name);
	}

	virtual TSharedRef<ISlateRun> Create(
		const TSharedRef<FTextLayout>& TextLayout,
		const FTextRunParseResults& RunParseResult,
		const FString& OriginalText,
		const TSharedRef<FString>& InOutModelText,
		const ISlateStyle* Style) override
	{
		UDialogueTextBlock* DialogueBlock = Cast<UDialogueTextBlock>(Owner.Get());
		if (Owner.IsValid() && DialogueBlock && DialogueBlock->JustifyDecorator->IsMeasurePassOnly())
		{
			FTextRange ModelRange;
			ModelRange.BeginIndex = InOutModelText->Len();
			*InOutModelText += OriginalText.Mid(
				RunParseResult.ContentRange.BeginIndex,
				RunParseResult.ContentRange.EndIndex - RunParseResult.ContentRange.BeginIndex);
			ModelRange.EndIndex = InOutModelText->Len();

			FTextBlockStyle RunStyle = Owner.IsValid()
				? Owner->GetCurrentDefaultTextStyle()
				: FCoreStyle::Get().GetWidgetStyle<FTextBlockStyle>("Default");

			return FSlateTextRun::Create(
				FRunInfo(RunParseResult.Name),
				InOutModelText,
				RunStyle,
				ModelRange);
		}
		
		// Map tag name → alignment
        EHorizontalAlignment ResolvedAlignment = Alignment;
        if      (RunParseResult.Name == TEXT("Centered"))   ResolvedAlignment = HAlign_Center;
        else if (RunParseResult.Name == TEXT("Right"))      ResolvedAlignment = HAlign_Right;
        else if (RunParseResult.Name == TEXT("Left"))       ResolvedAlignment = HAlign_Left;
        else if (RunParseResult.Name == TEXT("Justified"))  ResolvedAlignment = HAlign_Fill;

        // Append the run's text into the model (required by the layout system)
        FTextRange ModelRange;
        ModelRange.BeginIndex = InOutModelText->Len();
        *InOutModelText += OriginalText.Mid(
            RunParseResult.ContentRange.BeginIndex,
            RunParseResult.ContentRange.EndIndex - RunParseResult.ContentRange.BeginIndex);
        ModelRange.EndIndex = InOutModelText->Len();

        // Grab text block width so our SBox can fill it
        float BlockWidth = 0.f;
        if (Owner.IsValid())
        {
            FVector2D Size = Owner->GetCachedGeometry().GetLocalSize();
            BlockWidth = Size.X;
            if (BlockWidth <= 0.f) BlockWidth = Owner->GetDesiredSize().X;
        }

        // Build the run's display text (stripped of tags)
        FString RunText = OriginalText.Mid(
            RunParseResult.ContentRange.BeginIndex,
            RunParseResult.ContentRange.EndIndex - RunParseResult.ContentRange.BeginIndex);

		TArray<TSharedRef<ITextDecorator>> NestedDecorators;
		if (Owner.IsValid())
		{
			for (URichTextBlockDecorator* Dec :
				static_cast<URichTextBlockExposeDecorators*>(Owner.Get())->GetDecorators())
			{
				if (Dec)
				{
					TSharedPtr<ITextDecorator> Inst = Dec->CreateDecorator(Owner.Get());
					if (Inst.IsValid())
						NestedDecorators.Add(Inst.ToSharedRef());
				}
			}
		}
		
        FTextBlockStyle FinalStyle = Owner.IsValid()
            ? Owner->GetCurrentDefaultTextStyle()
            : FCoreStyle::Get().GetWidgetStyle<FTextBlockStyle>("Default");

		ISlateStyle* OwnerStyleInstance = nullptr;
		if (Owner.IsValid())
		{
			// StyleInstance is protected on URichTextBlock, so cast to UDialogueTextBlock
			// which inherits it, and access it directly
			auto StyleInstance =
				static_cast<URichTextBlockExposeDecorators*>(Owner.Get())->GetStyleInstance();
			if (DialogueBlock && StyleInstance.IsValid())
			{
				OwnerStyleInstance = StyleInstance.Get();
			}
		}

		TSharedRef<SWidget> AlignedWidget =
		SNew(SBox)
			.WidthOverride(BlockWidth > 0.f ? BlockWidth : FOptionalSize())
			.HAlign(ResolvedAlignment)
		   [
			   SNew(SRichTextBlock)
				   .Text(FText::FromString(RunText))
				   .TextStyle(&FinalStyle)
				   .Decorators(NestedDecorators)
				   .DecoratorStyleSet(OwnerStyleInstance)
		   ];

        // Wrap in a widget run so the layout system can measure and place it
        FSlateWidgetRun::FWidgetRunInfo WidgetRunInfo(AlignedWidget, 0);

        return FSlateWidgetRun::Create(
            TextLayout,
            FRunInfo(RunParseResult.Name),
            InOutModelText,
            WidgetRunInfo,
            ModelRange);
	}

private:
	TWeakObjectPtr<URichTextBlock> Owner;
	TEnumAsByte<EHorizontalAlignment> Alignment;
};

UJustifyTextDecorator::UJustifyTextDecorator(
	const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer) {}

TSharedPtr<ITextDecorator> UJustifyTextDecorator::CreateDecorator(
	URichTextBlock* InOwner)
{
	return MakeShared<FJustifyDecorator>(InOwner, FallbackAlignment);
}
