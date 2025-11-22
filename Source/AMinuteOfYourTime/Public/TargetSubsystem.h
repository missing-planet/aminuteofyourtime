// Copyright (c) 2025, Missing Planet LLC. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Target.h"
#include "TargetData.h"
#include "Subsystems/EngineSubsystem.h"
#include "Containers/IndirectArray.h"
#include "TargetSubsystem.generated.h"

UDELEGATE()
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FUpdateLocationSignature, ETimeslot, CurrentTime, int32, CurrentDay);

UCLASS()
class AMINUTEOFYOURTIME_API UTargetSubsystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()

public:

	UFUNCTION(BlueprintCallable)
	UTarget* AddTarget(UTargetData* TargetData, UObject* WorldContextObject);

	UFUNCTION(BlueprintCallable, BlueprintPure)
	UTarget* GetTargetAtLocation(ELocation Location, bool& Success) const;

	UFUNCTION(BlueprintCallable, BlueprintPure)
	UTarget* GetTargetByName(FName TargetName, bool& Success) const;

	UFUNCTION(BlueprintCallable, BlueprintPure)
	bool HasTargetScheduleKnowledge(FName TargetName, FDateTimePair Knowledge) const;

	UFUNCTION(BlueprintCallable)
	void AddAllTargetKnowledge();

public:

	UPROPERTY(BlueprintAssignable)
	FUpdateLocationSignature UpdateLocationEvent;

private:

	UPROPERTY()
	TArray<UTarget*> Targets;
};
