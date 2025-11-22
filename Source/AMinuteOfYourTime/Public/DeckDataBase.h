// Copyright (c) 2025, Missing Planet LLC. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "Templates/SharedPointer.h"
#include "DeckDataBase.generated.h"

class UCardDataBase;
class UDeckDataBase;

UCLASS(DisplayName="Deck Data Base", Blueprintable, BlueprintType)
class AMINUTEOFYOURTIME_API UDeckDataBase : public UPrimaryDataAsset
{
	GENERATED_BODY()

public:
	virtual FPrimaryAssetId GetPrimaryAssetId() const override
	{
		return FPrimaryAssetId("DeckDataBase", GetFName());
	}

public:

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TArray<UCardDataBase*> CardList;
};
