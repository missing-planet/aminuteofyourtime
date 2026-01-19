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

	UPROPERTY(BlueprintReadWrite)
	int32 ActionPointOffset;

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
	float DestroyChanceOffset;

	UPROPERTY(BlueprintReadWrite)
	float DestroyChanceMultiplier = 1;

	UPROPERTY(BlueprintReadWrite)
	UDeckObjectBase* OwningDeck = nullptr;

	/*UPROPERTY(EditAnywhere)
	FGameplayTagContainer CardFlags;*/
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