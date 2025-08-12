#pragma once

#include "CoreMinimal.h"
#include "FMODEvent.h"
#include "Runtime/Engine/Classes/Engine/DataTable.h"
#include "SpeakerSettings.generated.h"

USTRUCT(BlueprintType)
struct FSpeakerSettings : public FTableRowBase
{
	GENERATED_USTRUCT_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FText SpeakerName;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TObjectPtr<UFMODEvent> SpeakerEvent;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FVector2D PitchRange = FVector2D(0.8f, 1.2f);

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	int32 Frequency = 1;
};

USTRUCT(BlueprintType)
struct FPortraitMapping : public FTableRowBase
{
	GENERATED_USTRUCT_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TObjectPtr<UTexture2D> Portrait;
};