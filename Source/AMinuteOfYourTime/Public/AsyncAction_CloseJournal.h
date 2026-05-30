// Copyright (c) 2025, Missing Planet LLC. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintAsyncActionBase.h"
#include "AsyncAction_CloseJournal.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FJournalCloseOutputPin);

UCLASS()
class AMINUTEOFYOURTIME_API UAsyncAction_CloseJournal : public UBlueprintAsyncActionBase
{
	GENERATED_BODY()

public:

	UPROPERTY(BlueprintAssignable)
	FJournalCloseOutputPin OnFinished;

	UPROPERTY(BlueprintAssignable)
	FJournalCloseOutputPin OnSkipped;

	UFUNCTION(BlueprintCallable,
		meta = (BlueprintInternalUseOnly = "true", WorldContext = "WorldContextObject"))
	static UAsyncAction_CloseJournal* CloseJournalAsync(UObject* WorldContextObject);
	
	virtual void Activate() override;

private:

	UObject* WorldContext;

	UFUNCTION()
	void HandleCloseFinished();

	UFUNCTION()
	void HandleCloseSkipped();

	void CleanUp();
};
