// Copyright (c) 2025, Missing Planet LLC. All rights reserved.


#include "Core/CardGameMode.h"

#include "CardPlayerState.h"

int32 ACardGameMode::DrawCardsToHand(
	UDeckObjectBase* Deck, const TScriptInterface<IPlayerHandInterface>& Hand,
	FVector2D DrawLocation, int32 Count, float Delay)
{
	if (!Deck || Deck->IsDeckLocked()) return 0;

	if (Deck->GetCardCount() <= 0)
	{
		PLATFORM_BREAK();
	}
	TArray<UCardDataBase*> Cards;
	int32 Amount = Deck->DrawCards(Count, Cards);
	Deck->DeckCountChangeEvent.Broadcast(Deck->GetCardCount());

	IPlayerHandInterface::Execute_AddCards(Hand.GetObject(), Cards, DrawLocation, Delay, true);

	return Amount;
}
