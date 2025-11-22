// Copyright (c) 2025, Missing Planet LLC. All rights reserved.


#include "Target.h"

#include <string>

#include "TargetData.h"
#include "TargetScheduleRow.h"
#include "TargetSubsystem.h"
#include "Kismet/GameplayStatics.h"

void UTarget::Initialize(const UTargetData* TargetData, const UObject* WorldContextObject)
{
	if (!TargetData)
	{
		UE_LOG(LogTemp, Error, TEXT("Invalid data pointer passed to Initialize Target!"));
		return;
	}

	TargetName = TargetData->TargetName;
	Schedule = TargetData->TargetSchedule;

	FTargetScheduleRow* DefaultSchedule = 
		TargetData->TargetSchedule->FindRow<FTargetScheduleRow>(FName("Default"), "Initial Load");

	if (DefaultSchedule)
	{
		CurrentLocation = DefaultSchedule->Timeslot1Location;
	}

	if (UGameInstance* GI = UGameplayStatics::GetGameInstance(WorldContextObject))
	{
		if (UTargetSubsystem* TargetSubsystem = GI->GetSubsystem<UTargetSubsystem>())
		{
			UE_LOG(LogTemp, Warning, TEXT("BOUND EVENT!"));
			TargetSubsystem->UpdateLocationEvent.AddDynamic(this, &UTarget::OnDateTimeChanged);
		}
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("FAILED TO BIND EVENT! INVALID TARGET SUB"));
		}
			
	} else
	{
		UE_LOG(LogTemp, Warning, TEXT("FAILED TO BIND EVENT! INVALID GI"));
	}
}

void UTarget::SetLocation(ELocation Location)
{
	CurrentLocation = Location;
}

ELocation UTarget::GetLocation() const
{
	return CurrentLocation;
}

FName UTarget::GetTargetName() const
{
	return TargetName;
}

void UTarget::AddScheduleKnowledge(FDateTimePair Knowledge)
{
	AcquiredScheduleKnowledge.AddUnique(Knowledge);
}

bool UTarget::HasScheduleKnowledge(FDateTimePair Knowledge) const
{
	return AcquiredScheduleKnowledge.Contains(Knowledge);
}

void UTarget::OnDateTimeChanged(ETimeslot CurrentTime, int32 CurrentDay)
{
	FString DayAsString = FString::FromInt(CurrentDay);

	FTargetScheduleRow* ScheduleRow =
		Schedule->FindRow<FTargetScheduleRow>(FName(DayAsString), "DateTime Changed");

	if (ScheduleRow)
	{
		switch (CurrentTime)
		{
		case ETimeslot::Morning:
			CurrentLocation = ScheduleRow->Timeslot1Location;
			break;
		case ETimeslot::Noon:
			CurrentLocation = ScheduleRow->Timeslot2Location;
			break;
		case ETimeslot::Night:
			CurrentLocation = ScheduleRow->Timeslot3Location;
			break;
		case ETimeslot::Count:
			break;
		}
	}

	UE_LOG(LogTemp, Warning, TEXT("Target %s location: %s"), *TargetName.ToString(), *UEnum::GetDisplayValueAsText(CurrentLocation).ToString());
}
