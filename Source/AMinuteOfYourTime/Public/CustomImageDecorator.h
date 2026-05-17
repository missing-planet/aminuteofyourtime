// Copyright (c) 2025, Missing Planet LLC. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Components/RichTextBlockDecorator.h"
#include "Components/RichTextBlockImageDecorator.h"
#include "CustomImageDecorator.generated.h"

/**
 * 
 */
UCLASS(Abstract, Blueprintable)
class AMINUTEOFYOURTIME_API UCustomImageDecorator : public URichTextBlockDecorator
{
	GENERATED_BODY()

public:
	UCustomImageDecorator(const FObjectInitializer& ObjectInitializer);

	virtual TSharedPtr<ITextDecorator> CreateDecorator(URichTextBlock* InOwner) override;

	virtual const FSlateBrush* FindImageBrush(FName TagOrId, bool bWarnIfMissing);

	void TickProgress(float DeltaTime);

protected:

	FRichImageRow* FindImageRow(FName TagOrId, bool bWarnIfMissing);

	UPROPERTY(EditAnywhere, Category=Appearance, meta = (RequiredAssetDataTags = "RowStructure=/Script/UMG.RichImageRow"))
	TObjectPtr<class UDataTable> ImageSet;

	UPROPERTY()
	float Progress = 0.0f;

	/*UPROPERTY()
	FString RowID;*/
};
