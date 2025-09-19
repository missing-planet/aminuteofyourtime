// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "CardType.h"
#include "GameplayTagContainer.h"
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

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	ECardType CardType;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float DestroyChance;

	UPROPERTY(BlueprintReadWrite)
	UDeckObjectBase* OwningDeck = nullptr;

	/*UPROPERTY(EditAnywhere)
	FGameplayTagContainer CardFlags;*/
};

