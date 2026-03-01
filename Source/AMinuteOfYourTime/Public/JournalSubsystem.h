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
	UUserWidget* PushPage(TSubclassOf<UUserWidget> PageType);

	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable)
	UUserWidget* PushPageEx(TSubclassOf<UUserWidget> PageType, const FString& EventID, bool bDelayUntilJournalOpen);

	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable)
	UUserWidget* PopPage(TSubclassOf<UUserWidget> PopAsType);

	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable)
	void ClearPages();

	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable)
	TArray<UUserWidget*> GetPages() const;

	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable)
	void CloseJournal();

	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable)
	void NavigateForward();

	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable)
	void NavigateBack();

	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable)
	void SetCanSkip(bool bCanSkip);

	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable)
	int32 GetPageCount();

	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable)
	bool IsPageFinalEventHandler(const UUserWidget* Page);
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

	UFUNCTION(BlueprintCallable, meta = (DeterminesOutputType = "PageType"))
	UObject* PushPage(TSubclassOf<UUserWidget> PageType);

	UFUNCTION(BlueprintCallable, meta = (DeterminesOutputType = "PageType"))
	UObject* PushPageEx(TSubclassOf<UUserWidget> PageType, const FString& EventID, bool bDelayUntilJournalOpen);

	UFUNCTION(BlueprintCallable)
	TArray<UUserWidget*> PushPages(TArray<TSubclassOf<UUserWidget>> PageTypes);

	UFUNCTION(BlueprintCallable, meta = (DeterminesOutputType = "PopAsType"))
	UObject* PopPage(TSubclassOf<UUserWidget> PopAsType);

	UFUNCTION(BlueprintCallable, BlueprintPure)
	TArray<UUserWidget*> GetPages();

	UFUNCTION(BlueprintCallable)
	void ClearPages();

	UFUNCTION(BlueprintCallable)
	void NavigateForward();

	UFUNCTION(BlueprintCallable)
	void NavigateBack();

	UFUNCTION(BlueprintCallable)
	void SetCanSkip(bool bCanSkip);

	UFUNCTION(BlueprintCallable, meta = (DeterminesOutputType = "InType"))
	UObject* PushPageAndStartPath(TSubclassOf<UUserWidget> InType, const FString& Path);

	UFUNCTION(BlueprintCallable)
	TArray<UUserWidget*> PushPagesAndStartPath(TArray<TSubclassOf<UUserWidget>> InTypes, const FString& Path);

	UFUNCTION(BlueprintCallable)
	void CloseJournal();

	UFUNCTION(BlueprintCallable)
	int32 GetPageCount() const;

	UFUNCTION(BlueprintCallable, BlueprintPure)
	bool IsPageFinalEventHandler(const UUserWidget* Page);

private:

	UPROPERTY()
	TScriptInterface<IJournalInterface> Journal;
};
