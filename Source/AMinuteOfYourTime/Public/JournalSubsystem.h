// Copyright (c) 2025, Missing Planet LLC. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "JournalSubsystem.generated.h"

UINTERFACE(MinimalAPI, Blueprintable)
class UJournalInterface : public UInterface
{
	GENERATED_BODY()
};

class IJournalInterface
{
	GENERATED_BODY()

public:

	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable)
	UUserWidget* ChangePage(TSubclassOf<UUserWidget> PageType);

	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable)
	void CloseJournal();
};

UCLASS()
class AMINUTEOFYOURTIME_API UJournalSubsystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()

public:

	UFUNCTION(BlueprintCallable, BlueprintPure)
	static UJournalSubsystem* GetJournalSubsystem(UObject* WorldContext);

	UFUNCTION(BlueprintCallable)
	void SetJournalWidget(TScriptInterface<IJournalInterface> InJournal);

	UFUNCTION(BlueprintCallable, meta = (DeterminesOutputType = "InType"))
	UObject* ChangePage(TSubclassOf<UUserWidget> InType);

	UFUNCTION(BlueprintCallable, meta = (DeterminesOutputType = "InType"))
	UObject* ChangePageAndStartPath(TSubclassOf<UUserWidget> InType, const FString& Path);

	UFUNCTION(BlueprintCallable)
	void CloseJournal();

private:

	UPROPERTY()
	TScriptInterface<IJournalInterface> Journal;
};
