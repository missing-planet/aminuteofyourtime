// Copyright (c) 2025, Missing Planet LLC. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Inkpot/InkpotValue.h"
#include "Subsystems/EngineSubsystem.h"
#include "InkStorySubsystem.generated.h"

class UInkpotChoice;
class UInkpotStoryAsset;
class UInkpotStory;

UDELEGATE(BlueprintCallable)
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FPathStartedSignature, const FString&, PathName, UObject*, CurrentHandler);

UDELEGATE(BlueprintCallable)
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FPathEndReachedSignature, const FString&, PathName, UObject*, CurrentHandler);

UCLASS()
class AMINUTEOFYOURTIME_API UInkStorySubsystem : public UEngineSubsystem
{
	GENERATED_BODY()
	
public:

	UFUNCTION(BlueprintCallable)
	bool StartStory(UInkpotStoryAsset* InStory);

	UFUNCTION(BlueprintCallable)
	bool ContinueStory();

	UFUNCTION(BlueprintCallable)
	void MakeChoice(UInkpotChoice* Choice);

	UFUNCTION(BlueprintCallable)
	void MakeChoiceIndex(int32 Index);

	UFUNCTION(BlueprintCallable)
	void StartPath(const FString& Path);

	UFUNCTION(BlueprintCallable)
	void StartPathWithParameters(const FString& Path, const TArray<FInkpotValue>& Params);

	UFUNCTION(BlueprintPure)
	UInkpotStory* GetStory() const { return Story; }

	UFUNCTION(BlueprintPure)
	FString GetCurrentPath() const { return CurrentPath; }

	UFUNCTION(BlueprintCallable)
	void SetCurrentStoryHandler(UObject* Handler) { CurrentStoryHandler = Handler; }

	UFUNCTION(BlueprintCallable, BlueprintPure)
	UObject* GetCurrentStoryHandler() const { return CurrentStoryHandler.Get(); }

	UFUNCTION(BlueprintCallable, BlueprintPure, meta = (DeterminesOutputType = "InType"))
	UObject* GetCurrentStoryHandlerAs(TSubclassOf<UObject> InType) const
	{
		return CurrentStoryHandler.Get();
	}

	UFUNCTION(BlueprintCallable, BlueprintPure)
	bool HasStoryBegun() const { return bStoryHasBegun; }

public:

	UPROPERTY(BlueprintAssignable)
	FPathEndReachedSignature PathEndReachedEvent;

	UPROPERTY(BlueprintAssignable)
	FPathStartedSignature PathStartedEvent;

	UPROPERTY(BlueprintAssignable, BlueprintCallable)
	FPathEndReachedSignature EventEndReachedEvent;

private:

	UPROPERTY()
	UInkpotStory* Story;

	UPROPERTY()
	FString CurrentPath;

	UPROPERTY()
	TObjectPtr<UObject> CurrentStoryHandler;

	UPROPERTY()
	bool bStoryHasBegun = false;
};
