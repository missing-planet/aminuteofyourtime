// Copyright (c) 2025, Missing Planet LLC. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Components/RichTextBlockDecorator.h"
#include "JustifyTextDecorator.generated.h"

USTRUCT()
struct FJustifyTextDecoratorSettings : public FTableRowBase
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere)
	TEnumAsByte<EHorizontalAlignment> Alignment = HAlign_Center;
};

UCLASS()
class AMINUTEOFYOURTIME_API UJustifyTextDecorator : public URichTextBlockDecorator
{
	GENERATED_BODY()

public:
	UJustifyTextDecorator(const FObjectInitializer& ObjectInitializer);

	virtual TSharedPtr<ITextDecorator> CreateDecorator(URichTextBlock* InOwner) override;

	void SetMeasurePassOnly(bool bInMeasurePass) {bMeasurePassOnly = bInMeasurePass;}
	bool IsMeasurePassOnly() const {return bMeasurePassOnly;}

protected:
	UPROPERTY(EditAnywhere, Category="Justify")
	TEnumAsByte<EHorizontalAlignment> FallbackAlignment = HAlign_Center;

	bool bMeasurePassOnly = false;
};
