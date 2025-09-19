// Fill out your copyright notice in the Description page of Project Settings.


#include "Core/CardGameMode.h"

int32 ACardGameMode::DrawCardsToHand(
	UDeckObjectBase* Deck, TScriptInterface<IPlayerHandInterface> Hand,
	FVector2D DrawLocation, int32 Count, float Delay)
{
	if (!Deck || Deck->IsDeckLocked()) return 0;

	int32 InitialCount = Deck->GetCardCount();

	for (int32 i = 0; i < Count; ++i)
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
	}

	return FMath::Min(InitialCount, Count);
}
