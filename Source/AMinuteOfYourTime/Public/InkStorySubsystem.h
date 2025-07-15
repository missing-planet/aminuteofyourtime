// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Inkpot/InkpotValue.h"
#include "Subsystems/EngineSubsystem.h"
#include "InkStorySubsystem.generated.h"

class UInkpotChoice;
class UInkpotStoryAsset;
class UInkpotStory;

UDELEGATE(BlueprintCallable)
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FPathStartedSignature, const FString&, PathName);

UDELEGATE(BlueprintCallable)
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FPathEndReachedSignature, const FString&, PathName);

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

public:

	UPROPERTY(BlueprintAssignable)
	FPathEndReachedSignature PathEndReachedEvent;

	UPROPERTY(BlueprintAssignable)
	FPathStartedSignature PathStartedEvent;

private:

	UPROPERTY()
	UInkpotStory* Story;

	UPROPERTY()
	FString CurrentPath;
};
