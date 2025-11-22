// Copyright (c) 2025, Missing Planet LLC. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Location.h"
#include "Timeslot.h"
#include "UObject/NoExportTypes.h"
#include "Target.generated.h"

class UTargetData;

UCLASS()
class AMINUTEOFYOURTIME_API UTarget : public UObject
{
	GENERATED_BODY()

public:

	void Initialize(const UTargetData* TargetData, const UObject* WorldContextObject);

	void SetLocation(ELocation Location);

	UFUNCTION(BlueprintCallable)
	ELocation GetLocation() const;
	UFUNCTION(BlueprintCallable)
	FName GetTargetName() const;

	UFUNCTION(BlueprintCallable)
	void AddScheduleKnowledge(FDateTimePair Knowledge);

	UFUNCTION(BlueprintCallable)
	bool HasScheduleKnowledge(FDateTimePair Knowledge) const;

private:

	UFUNCTION()
	void OnDateTimeChanged(ETimeslot CurrentTime, int32 CurrentDay);

private:

	FName TargetName;
	ELocation CurrentLocation;

	UPROPERTY()
	UDataTable* Schedule;

	TArray<FDateTimePair> AcquiredScheduleKnowledge;
};
