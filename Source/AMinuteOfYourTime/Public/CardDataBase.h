// Copyright (c) 2025, Missing Planet LLC. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "CardDefines.h"
#include "CardType.h"
#include "GameplayTagContainer.h"
#include "Components/CanvasPanel.h"
#include "Engine/DataAsset.h"
#include "CardDataBase.generated.h"

class UDeckObjectBase;

UCLASS(DisplayName="Card Data Base", Blueprintable, BlueprintType)
class AMINUTEOFYOURTIME_API UCardDataBase : public UPrimaryDataAsset
{
	GENERATED_BODY()

public:
	virtual FPrimaryAssetId GetPrimaryAssetId() const override
	{
		return FPrimaryAssetId("CardDataBase", GetFName());
	}

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 ActionPointCost;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FText CardName;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FText CardDescription;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	ECardType CardType;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	ECard Card;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float DestroyChance;

	UPROPERTY(BlueprintReadWrite)
	UDeckObjectBase* OwningDeck = nullptr;

	/*UPROPERTY(EditAnywhere)
	FGameplayTagContainer CardFlags;*/
};

UCLASS(BlueprintType)
class AMINUTEOFYOURTIME_API UCardDataRuntime : public UObject
{
	GENERATED_BODY()
public:
	UCardDataRuntime() = default;

	UCardDataRuntime(const UCardDataBase& CardData)
		: ActionPointCost(CardData.ActionPointCost)
		, CardName(CardData.CardName)
		, CardDescription(CardData.CardDescription)
		, CardType(CardData.CardType)
		, Card(CardData.Card)
		, DestroyChance(CardData.DestroyChance)
		, OwningDeck(CardData.OwningDeck) {}

	UFUNCTION(BlueprintCallable)
	void Initialize(const UCardDataBase* CardData)
	{
		if (!CardData) return;
		
		ActionPointCost = CardData->ActionPointCost;
		CardName = CardData->CardName;
		CardDescription = CardData->CardDescription;
		CardType = CardData->CardType;
		Card = CardData->Card;
		DestroyChance = CardData->DestroyChance;
		OwningDeck = CardData->OwningDeck;
	}

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 ActionPointCost;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 ActionPointOffset = 0;

	UPROPERTY(BlueprintReadWrite)
	int32 ActionPointMultiplier = 1;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FText CardName;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FText CardDescription;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	ECardType CardType;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	ECard Card;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float DestroyChance;

	UPROPERTY(BlueprintReadWrite)
	float DestroyChanceOffset = 0;

	UPROPERTY(BlueprintReadWrite)
	float DestroyChanceMultiplier = 1;

	UPROPERTY(BlueprintReadWrite)
	UDeckObjectBase* OwningDeck = nullptr;
};

inline TArray<UCanvasPanel*> g_CanvasPanelStack;

UCLASS(Blueprintable)
class AMINUTEOFYOURTIME_API UCardBlueprintExtensions : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()
	
public:
	UFUNCTION(BlueprintCallable)
	static void PushCardAnimationCanvas(UCanvasPanel* Canvas)
	{
		g_CanvasPanelStack.Push(Canvas);
	}

	UFUNCTION(BlueprintCallable)
	static void PopCardAnimationCanvas()
	{
		if (!g_CanvasPanelStack.IsEmpty())
			g_CanvasPanelStack.Pop();
	}

	UFUNCTION(BlueprintCallable, BlueprintPure)
	static const UCanvasPanel* GetCardAnimationCanvas()
	{
		if (g_CanvasPanelStack.IsEmpty()) return nullptr;
		
		return g_CanvasPanelStack.Last();
	}
};