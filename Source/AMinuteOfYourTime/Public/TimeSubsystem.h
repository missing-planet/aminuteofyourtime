// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Timeslot.h"
#include "Subsystems/EngineSubsystem.h"
#include "TimeSubsystem.generated.h"

UDELEGATE()
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FDateTimeChangedSignature, ETimeslot, CurrentTime, int32, CurrentDay);

UCLASS()
class AMINUTEOFYOURTIME_API UTimeSubsystem : public UEngineSubsystem
{
	GENERATED_BODY()

public:

	UFUNCTION(BlueprintCallable)
	void SetCurrentTime(ETimeslot NewTime);

	UFUNCTION(BlueprintCallable)
	void IncreaseTime(uint8 Amount = 1);

	UFUNCTION(BlueprintCallable)
	void DecreaseTime(uint8 Amount = 1);

	UFUNCTION(BlueprintCallable)
	void SetCurrentDay(int32 Day);

	UFUNCTION(BlueprintCallable)
	void IncreaseDay(int32 Amount = 1);

	UFUNCTION(BlueprintCallable)
	void DecreaseDay(int32 Amount = 1);

	UFUNCTION(BlueprintPure)
	ETimeslot GetCurrentTime() const;

	UFUNCTION(BlueprintPure)
	int32 GetCurrentDay() const;

	UFUNCTION(BlueprintPure)
	FDateTimePair GetCurrentDateTime() const { return FDateTimePair(CurrentTime, CurrentDay); }

public:

	UPROPERTY(BlueprintAssignable)
	FDateTimeChangedSignature DateTimeChangedEvent;

private:

	ETimeslot CurrentTime = ETimeslot::Morning;

	int32 CurrentDay = 1;
};
