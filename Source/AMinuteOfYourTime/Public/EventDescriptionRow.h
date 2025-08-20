// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include <Components/Widget.h>

#include "CoreMinimal.h"
#include "Runtime/UMG/Public/Blueprint/UserWidget.h"
#include "Timeslot.h"
#include "EventDescriptionRow.generated.h"

USTRUCT()
struct FEventDescriptionRow : public FTableRowBase 
{
	GENERATED_USTRUCT_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FName EventTitle;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (MultiLine="true"))
	FText EventDescription;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TArray<FDateTimePair> EventTimes;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FSlateBrush EventImage;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TSubclassOf<UObject> EventClass;
};

UCLASS()
class UBlueprintEventFunctionLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, BlueprintPure)
	static UObject* GetEvent(const FEventDescriptionRow& EventDescription, bool& Result);

	UFUNCTION(BlueprintCallable, BlueprintPure)
	static FEventDescriptionRow GetEventForTime(const UDataTable* EventData, FDateTimePair DateTime);
};
