// Fill out your copyright notice in the Description page of Project Settings.


#include "Core/CardGameMode.h"

int32 ACardGameMode::DrawCardsToHand(
	UDeckObjectBase* Deck, TScriptInterface<IPlayerHandInterface> Hand,
	FVector2D DrawLocation, int32 Count, float Delay)
{
	TArray<UCardDataBase*> Cards;

	int32 InitialCount = Deck->GetCardCount();

	for (int32 i = 0; i < Count; ++i)
	{
		FTimerHandle Handle;
		GetWorldTimerManager().SetTimer(Handle, [Deck, Hand, Cards, DrawLocation]()
			{
				Deck->DrawCards(1, const_cast<TArray<UCardDataBase*>&>(Cards));
				Deck->DeckCountChangeEvent.Broadcast(Deck->GetCardCount());
				IPlayerHandInterface::Execute_AddCard(Hand.GetObject(), Cards.Last(), DrawLocation);
			}, 0.001f, false, Delay * i);
	}

	return FMath::Min(InitialCount, Count);
}
