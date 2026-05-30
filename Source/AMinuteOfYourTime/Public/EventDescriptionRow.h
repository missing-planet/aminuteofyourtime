// Copyright (c) 2025, Missing Planet LLC. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "EventConditionBase.h"
#include "EventType.h"
#include "EventDescriptionRow.generated.h"

class UStoryEventBase;
class IStoryEventInterface;

USTRUCT()
struct FEventDescriptionRow : public FTableRowBase 
{
	GENERATED_USTRUCT_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FName EventTitle;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (MultiLine="true"))
	FText EventDescription;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FName EventID;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	EventType EventType = EventType::Other;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TArray<TSubclassOf<UEventConditionBase>> EventConditions;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FSlateBrush EventImage = FSlateBrush();

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TSubclassOf<UStoryEventBase> EventClass;

	bool operator==(const FEventDescriptionRow& other) const
	{
		return other.EventID == EventID;
	}
};

UCLASS()
class UBlueprintEventFunctionLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, BlueprintPure)
	static TScriptInterface<IStoryEventInterface> GetEvent(FEventDescriptionRow EventDescription, bool& Result);
	
	UFUNCTION(BlueprintCallable, BlueprintPure)
	static TArray<EventType> GetUniqueEventTypes(const TArray<FEventDescriptionRow>& Events);

	UFUNCTION(BlueprintCallable, BlueprintPure)
	static TArray<FEventDescriptionRow> GetAllowedEvents(const UDataTable* EventData);
	
	UFUNCTION(BlueprintCallable, BlueprintPure)
	static TArray<FEventDescriptionRow> FilterEventsByType(const TArray<FEventDescriptionRow>& Events, EventType Type);
};
