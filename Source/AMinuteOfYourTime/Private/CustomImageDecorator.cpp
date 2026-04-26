// Copyright (c) 2025, Missing Planet LLC. All rights reserved.


#include "CustomImageDecorator.h"

#include "Fonts/FontMeasure.h"
#include "Kismet/GameplayStatics.h"
#include "Misc/DefaultValueHelper.h"
#include "Widgets/Layout/SScaleBox.h"

#define LOCTEXT_NAMESPACE "AMINUTEOFYOURTIME"

class SCustomRichInlineImage  : public SCompoundWidget
{
public:
	SLATE_BEGIN_ARGS(SCustomRichInlineImage )
		: _WorldContext(nullptr)
	{}
		SLATE_ATTRIBUTE(UObject*, WorldContext)
	SLATE_END_ARGS()

public:
	void Construct(const FArguments& InArgs, const FSlateBrush* Brush, const FTextBlockStyle& TextStyle,
		TOptional<int32> Width, TOptional<int32> Height, EStretch::Type Stretch)
	{
		UE_LOG(LogTemp, Warning, TEXT("CONSTRUCT IS BEING RUN AT ALL!!"));
		check(Brush);

		UObject* Resource = Brush->GetResourceObject();
		UMaterialInterface* SourceMat = nullptr;

		if (UMaterialInstanceDynamic* ExistingMID = Cast<UMaterialInstanceDynamic>(Resource))
		{
			SourceMat = ExistingMID->Parent;
		}
		else if (UMaterialInterface* MatInterface = Cast<UMaterialInterface>(Resource))
		{
			SourceMat = MatInterface;
		}

		if (SourceMat)
		{
			MID = UMaterialInstanceDynamic::Create(SourceMat, SourceMat->GetOuter());
			const_cast<FSlateBrush*>(Brush)->SetResourceObject(MID);
		}

		if (UObject* WorldContext = InArgs._WorldContext.Get())
		{
			WorldContextRef = WorldContext;
			if (UWorld* World = WorldContext->GetWorld())
			{
				World->GetTimerManager().SetTimer(TimerHandle, [this]()
				{
					if (MID)
					{
						float CurrentValue = 0.f;
						FHashedMaterialParameterInfo ParamInfo(TEXT("Progress"));
						MID->GetScalarParameterValue(ParamInfo, CurrentValue);
						MID->SetScalarParameterValue(TEXT("Progress"), CurrentValue + 0.0167f);
						UE_LOG(LogTemp, Warning, TEXT("PROGRESS: %f"), CurrentValue + 0.0167f);
					}
				}, 0.0167f, true);
			}
		}

		const TSharedRef<FSlateFontMeasure> FontMeasure =
			FSlateApplication::Get().GetRenderer()->GetFontMeasureService();
		float IconHeight = FMath::Min((float)FontMeasure->GetMaxCharacterHeight(TextStyle.Font, 1.0f),
			Brush->ImageSize.Y);

		if (Height.IsSet())
		{
			IconHeight = static_cast<float>(Height.GetValue());
		}

		float IconWidth = IconHeight;
		if (Width.IsSet())
		{
			IconWidth = static_cast<float>(Width.GetValue());
		}

		ChildSlot
		[
			SNew(SBox)
			.HeightOverride(IconHeight)
			.WidthOverride(IconWidth)
			[
				SNew(SScaleBox)
				.Stretch(Stretch)
				.StretchDirection(EStretchDirection::DownOnly)
				.VAlign(VAlign_Center)
				[
					SNew(SImage)
					.Image(Brush)
				]
			]
		];
	}

	virtual ~SCustomRichInlineImage() override
	{
		// Clean up the timer when the widget is destroyed
		if (WorldContextRef.IsValid())
		{
			if (UWorld* World = WorldContextRef->GetWorld())
			{
				World->GetTimerManager().ClearTimer(TimerHandle);
			}
		}
	}

private:
	UMaterialInstanceDynamic* MID = nullptr;
	FTimerHandle TimerHandle;
	TWeakObjectPtr<UObject> WorldContextRef;
};


/** 
 * Add an image inline with the text.
 * Usage: Before image <img id="MyId"/>, after image.
 * 
 * A width and height can be specified.
 * By default the width and the height is the same size as the font height.
 * Use "desired" to use the same size as the image brush.
 * Usage: Before image <img id="MyId" height="40" width="desired"/>, after image.
 * 
 * A stretch type can be specified. See EStretch.
 * By default the stretch type is ScaleToFit.
 * Usage: Before image <img id="MyId" stretch="ScaleToFitY"/>, after image.
 */
class FRichInlineImage : public FRichTextDecorator
{
public:
	FRichInlineImage(URichTextBlock* InOwner, UCustomImageDecorator* InDecorator, UObject* InWorldContext/*,
		const FString& InRowId*/)
		: FRichTextDecorator(InOwner)
		, Decorator(InDecorator)
		, WorldContext(InWorldContext)
		/*, RowId(InRowId)*/
	{
	}

	virtual bool Supports(const FTextRunParseResults& RunParseResult, const FString& Text) const override
	{
		if (RunParseResult.Name == TEXT("img") && RunParseResult.MetaData.Contains(TEXT("id")))
		{
			const FTextRange& IdRange = RunParseResult.MetaData[TEXT("id")];
			const FString TagId = Text.Mid(IdRange.BeginIndex, IdRange.EndIndex - IdRange.BeginIndex);
			const bool bWarnIfMissing = false;
			return Decorator->FindImageBrush(*TagId, bWarnIfMissing) != nullptr;
		}

		return false;
	}

protected:
	virtual TSharedPtr<SWidget> CreateDecoratorWidget(const FTextRunInfo& RunInfo, const FTextBlockStyle& TextStyle) const override
	{
		const bool bWarnIfMissing = true;
		const FSlateBrush* Brush = Decorator->FindImageBrush(*RunInfo.MetaData[TEXT("id")], bWarnIfMissing);

		if (ensure(Brush))
		{
			TOptional<int32> Width;
			if (const FString* WidthString = RunInfo.MetaData.Find(TEXT("width")))
			{
				int32 WidthTemp;
				if (FDefaultValueHelper::ParseInt(*WidthString, WidthTemp))
				{
					Width = WidthTemp;
				}
				else
				{
					if (FCString::Stricmp(GetData(*WidthString), TEXT("desired")) == 0)
					{
						Width = FMath::TruncToInt32(Brush->ImageSize.X);
					}
				}
			}

			TOptional<int32> Height;
			if (const FString* HeightString = RunInfo.MetaData.Find(TEXT("height")))
			{
				int32 HeightTemp;
				if (FDefaultValueHelper::ParseInt(*HeightString, HeightTemp))
				{
					Height = HeightTemp;
				}
				else
				{
					if (FCString::Stricmp(GetData(*HeightString), TEXT("desired")) == 0)
					{
						Height = FMath::TruncToInt32(Brush->ImageSize.Y);
					}
				}
			}

			EStretch::Type Stretch = EStretch::ScaleToFit;
			if (const FString* SstretchString = RunInfo.MetaData.Find(TEXT("stretch")))
			{
				const UEnum* StretchEnum = StaticEnum<EStretch::Type>();
				int64 StretchValue = StretchEnum->GetValueByNameString(*SstretchString);
				if (StretchValue != INDEX_NONE)
				{
					Stretch = static_cast<EStretch::Type>(StretchValue);
				}
			}

			return SNew(SCustomRichInlineImage, Brush, TextStyle, Width, Height, Stretch).WorldContext(WorldContext);
		}
		return TSharedPtr<SWidget>();
	}

private:
	UCustomImageDecorator* Decorator;
	UObject* WorldContext;
	/*FString RowId;*/
};

/////////////////////////////////////////////////////
// UCustomImageDecorator

UCustomImageDecorator::UCustomImageDecorator(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
}

const FSlateBrush* UCustomImageDecorator::FindImageBrush(FName TagOrId, bool bWarnIfMissing)
{
	const FRichImageRow* ImageRow = FindImageRow(TagOrId, bWarnIfMissing);
	if (ImageRow)
	{
		return &ImageRow->Brush;
	}

	return nullptr;
}

void UCustomImageDecorator::TickProgress(float DeltaTime)
{
	/*Progress = FMath::Min(Progress + DeltaTime, 1.0f);
    
	// Update the MID directly
	if (const FRichImageRow* Row = FindImageRow(TEXT("TradeOffer"), false))
	{
		UObject* Resource = Row->Brush.GetResourceObject();
		if (UMaterialInstanceDynamic* MID = Cast<UMaterialInstanceDynamic>(Resource))
		{
			MID->SetScalarParameterValue(TEXT("Progress"), Progress);
		}
	}*/
}

FRichImageRow* UCustomImageDecorator::FindImageRow(FName TagOrId, bool bWarnIfMissing)
{
	if (ImageSet)
	{
		FString ContextString;
		return ImageSet->FindRow<FRichImageRow>(TagOrId, ContextString, bWarnIfMissing);
	}
	
	return nullptr;
}

TSharedPtr<ITextDecorator> UCustomImageDecorator::CreateDecorator(URichTextBlock* InOwner)
{
	return MakeShareable(new FRichInlineImage(InOwner, this, this/*, RowID*/));
}

/////////////////////////////////////////////////////

#undef LOCTEXT_NAMESPACE