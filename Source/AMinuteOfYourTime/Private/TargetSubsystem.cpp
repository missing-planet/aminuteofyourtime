// Copyright (c) 2025, Missing Planet LLC. All rights reserved.


#include "TargetSubsystem.h"

UTarget* UTargetSubsystem::AddTarget(UTargetData* TargetData, UObject* WorldContextObject)
{
	if (UTarget* Target = NewObject<UTarget>(UTarget::StaticClass()))
	{
		Target->Initialize(TargetData, WorldContextObject);
		Targets.Add(Target);
		return Target;
	}
	else return nullptr;
}

UTarget* UTargetSubsystem::GetTargetAtLocation(ELocation Location, bool& Success) const
{
	for (auto& Target : Targets)
	{
		if (Target->GetLocation() == Location)
		{
			Success = true;
			return Target;
		}
	}

	Success = false;
	return nullptr;
}

UTarget* UTargetSubsystem::GetTargetByName(FName TargetName, bool& Success) const
{
	for (auto& Target : Targets)
	{
		if (Target->GetTargetName() == TargetName)
		{
			Success = true;
			return Target;
		}
	}

	Success = false;
	return nullptr;
}

bool UTargetSubsystem::HasTargetScheduleKnowledge(FName TargetName, FDateTimePair Knowledge) const
{
	bool FoundTarget = false;
	UTarget* Target = GetTargetByName(TargetName, FoundTarget);
	if (FoundTarget)
	{
		return Target->HasScheduleKnowledge(Knowledge);
	}

	return false;
}

void UTargetSubsystem::AddAllTargetKnowledge()
{
#ifdef UE_BUILD_DEVELOPMENT
	for (auto& Target : Targets)
	{
		for (int32 i = 1; i < 8; ++i)
		{
			for (uint8 t = 0; t < (uint8)ETimeslot::Count; ++t)
			{
				Target->AddScheduleKnowledge(FDateTimePair((ETimeslot)t, i));
			}
		}
	}
#endif
}
