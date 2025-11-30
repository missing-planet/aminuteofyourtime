// Copyright (c) 2025, Missing Planet LLC. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "CardInterface.h"
#include "DeckObjectBase.h"
#include "GameFramework/GameModeBase.h"
#include "CardGameMode.generated.h"

UCLASS()
class AMINUTEOFYOURTIME_API ACardGameMode : public AGameModeBase
{
	GENERATED_BODY()

public:

	UFUNCTION(BlueprintCallable)
	int32 DrawCardsToHand(UDeckObjectBase* Deck, const TScriptInterface<IPlayerHandInterface>& Hand,
		FVector2D DrawLocation, int32 Count, float Delay = 0.5f);
};
