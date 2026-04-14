// Copyright (c) 2025, Missing Planet LLC. All rights reserved.

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
	void AddCard(UCardDataRuntime* Card, FVector2D DrawLocation, float Delay, bool Broadcast = true);

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	void AddCards(const TArray<UCardDataRuntime*>& Cards, FVector2D DrawLocation, float Delay, bool Broadcast = true);

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	void RemoveCard(UCardDataRuntime* Card, bool Broadcast = true);
};
