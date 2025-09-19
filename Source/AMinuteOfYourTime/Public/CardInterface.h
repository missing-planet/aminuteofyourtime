// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "CardDataBase.h"
#include "UObject/Interface.h"
#include "CardInterface.generated.h"

class UDeckObjectBase;

UINTERFACE(MinimalAPI, Blueprintable)
class UCardInterface : public UInterface {
	GENERATED_BODY()
};

class ICardInterface {
	GENERATED_BODY()

public:

	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable)
	void ResetRenderTransform();

	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable)
	void SetShouldPlayAnimations(bool shouldPlay);

	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable)
	void SetLocked(bool locked);
};

UINTERFACE(MinimalAPI, Blueprintable)
class UPlayerHandInterface : public UInterface
{
	GENERATED_BODY()
};

class IPlayerHandInterface
{
	GENERATED_BODY()

public:

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	void AddCard(UCardDataBase* Card, FVector2D DrawLocation);

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	void AddCards(const TArray<UCardDataBase*>& Cards, FVector2D DrawLocation);

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	void RemoveCard(UCardDataBase* Card);
};
