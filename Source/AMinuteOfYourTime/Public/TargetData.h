// Copyright (c) 2025, Missing Planet LLC. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "TargetData.generated.h"

/**
 * 
 */
UCLASS(DisplayName = "Target Data", Blueprintable, BlueprintType)
class AMINUTEOFYOURTIME_API UTargetData : public UPrimaryDataAsset
{
	GENERATED_BODY()

public:
	virtual FPrimaryAssetId GetPrimaryAssetId() const override
	{
		return FPrimaryAssetId("TargetData", GetFName());
	}

public:

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FName TargetName;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	UDataTable* TargetSchedule;
};
