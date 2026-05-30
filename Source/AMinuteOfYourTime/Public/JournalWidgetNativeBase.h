// Copyright (c) 2025, Missing Planet LLC. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "JournalWidgetNativeBase.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FJournalEventSignature);

UCLASS()
class AMINUTEOFYOURTIME_API UJournalWidgetNativeBase : public UUserWidget
{
	GENERATED_BODY()

public:

	UPROPERTY(BlueprintAssignable, BlueprintCallable)
	FJournalEventSignature OnOpenFinished;

	UPROPERTY(BlueprintAssignable, BlueprintCallable)
	FJournalEventSignature OnOpenSkipped;

	UPROPERTY(BlueprintAssignable, BlueprintCallable)
	FJournalEventSignature OnCloseFinished;

	UPROPERTY(BlueprintAssignable, BlueprintCallable)
	FJournalEventSignature OnClosedSkipped;
};
