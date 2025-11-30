// Copyright (c) 2025, Missing Planet LLC. All rights reserved.


#include "Core/CardGameMode.h"

#include "CardPlayerState.h"

int32 ACardGameMode::DrawCardsToHand(
	UDeckObjectBase* Deck, const TScriptInterface<IPlayerHandInterface>& Hand,
	FVector2D DrawLocation, int32 Count, float Delay)
{
	if (!Deck || Deck->IsDeckLocked()) return 0;

	int32 InitialCount = Deck->GetCardCount();

	/*if (Count > 0)
	{
		FTimerHandle Handle;
		GetWorldTimerManager().SetTimer(Handle, [Hand]()
		{
			IPlayerHandInterface::Execute_FinalizeHand(Hand.GetObject());
		}, 0.001f, false, Delay * Count);
	}*/

	TArray<UCardDataBase*> Cards;
	int32 Amount = Deck->DrawCards(Count, Cards);
	Deck->DeckCountChangeEvent.Broadcast(Deck->GetCardCount());

	IPlayerHandInterface::Execute_AddCards(Hand.GetObject(), Cards, DrawLocation, Delay);

	/*for (int32 i = 0; i < Count; ++i)
	{
		if (!Deck->IsDeckLocked()) Deck->LockDeck(true);
		
		FTimerHandle Handle;
		GetWorldTimerManager().SetTimer(Handle, [Deck, Hand, DrawLocation, i, Count]()
			{
				TArray<UCardDataBase*> Cards;
				int32 Amount = Deck->DrawCards(1, Cards);
				if (Amount <= 0)
				{
					if (i == Count - 1) Deck->LockDeck(false);
					return;
				}
			
				Deck->DeckCountChangeEvent.Broadcast(Deck->GetCardCount());
				IPlayerHandInterface::Execute_AddCard(Hand.GetObject(), Cards.Last(), DrawLocation);
				if (i == Count - 1) Deck->LockDeck(false);
			}, 0.001f, false, Delay * i);
	}*/

	return FMath::Min(InitialCount, Count);
}
