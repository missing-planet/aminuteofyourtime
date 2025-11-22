// Copyright (c) 2025, Missing Planet LLC. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "StoryEventInterface.h"
#include "StoryEventBase.generated.h"

/**
 * 
 */
UCLASS(Blueprintable)
class AMINUTEOFYOURTIME_API UStoryEventBase : public UObject, public IStoryEventInterface
{
	GENERATED_BODY()

public:

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FString EventPath;

	virtual void Execute_Implementation(FEventDescriptionRow EventDescription, UObject* Caller) override;

	virtual void OnPathEndReached_Implementation(const FString& PathName, UObject* CurrentHandler) override;
	
};
