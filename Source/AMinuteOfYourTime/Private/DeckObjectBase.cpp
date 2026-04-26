// Copyright (c) 2025, Missing Planet LLC. All rights reserved.


#include "DeckObjectBase.h"

#include "CardDataBase.h"

void UDeckObjectBase::Initialize(UDeckDataBase* DeckData, FName Name, bool Shuffle)
{
	DeckName = Name;

	uint32 i = 0;
	for (const auto& [Card, Amount] : DeckData->CardList)
	{
		// Create a new object so that the original data asset can't be modified
		// Use deck name and index to prevent caching, we want a new card object for every card
		for (int j = 0; j < Amount; ++j)
		{
			UCardDataRuntime* NewCard = NewObject<UCardDataRuntime>();
			NewCard->Initialize(Card);
			NewCard->OwningDeck = this;
			Cards.Push(NewCard);
			CardCount++;
		}
	}

	if (Shuffle) this->Shuffle();

	DeckCountChangeEvent.Broadcast(DeckData->CardList.Num());
}

int UDeckObjectBase::DrawCards(int Count, TArray<UCardDataRuntime*>& OutCardList, bool BroadcastChange)
{
	int ReturnCount = 0;

	for (int i = 0; i < Count; ++i)
	{
		if (Cards.IsEmpty())
		{
			if (!DiscardDeck) break;

			TArray<UCardDataRuntime*> DiscardedCards;
			int32 Amount = DiscardDeck->DrawCards(DiscardDeck->GetCardCount(), DiscardedCards, false);
			DiscardDeck->DeckCountChangeEvent.Broadcast(Amount);
			AddCards(DiscardedCards);
			Shuffle();
		}

		if (Cards.IsEmpty()) continue;
		
		TObjectPtr<UCardDataRuntime> Card = Cards.Pop();
		
		OutCardList.Add(Card);
		OutOfDeckCards.Add(Card);
		ReturnCount++;

		CardCount--;

		if (BroadcastChange) DeckCountChangeEvent.Broadcast(1);
	}

	return ReturnCount;
}

void UDeckObjectBase::AddCard(UCardDataRuntime* Card)
{
	if (!Card) return;
	
	Cards.Push(Card);
	if (OutOfDeckCards.Contains(Card)) OutOfDeckCards.RemoveSingle(Card);

	CardCount++;
	DeckCountChangeEvent.Broadcast(1);
}

void UDeckObjectBase::AddCards(const TArray<UCardDataRuntime*>& CardList)
{
	for (UCardDataRuntime* card : CardList)
	{
		AddCard(card);
	}
}

void UDeckObjectBase::RemoveCard(UCardDataRuntime* Card)
{
	if (!Card) return;
	
	uint32 Removed = Cards.RemoveSingle(Card);
	if (OutOfDeckCards.Contains(Card)) OutOfDeckCards.RemoveSingle(Card);

	CardCount -= Removed;
	DeckCountChangeEvent.Broadcast(Removed);
}

void UDeckObjectBase::RemoveCards(const TArray<UCardDataRuntime*>& CardList)
{
	for (UCardDataRuntime* card : CardList)
	{
		RemoveCard(card);
	}
}

void UDeckObjectBase::Shuffle()
{
	TArray<UCardDataRuntime*> CardList;

	while (!Cards.IsEmpty())
	{
		TObjectPtr<UCardDataRuntime> Card = Cards.Pop();
		CardList.Add(std::move(Card));
	}

	const int32 NumShuffles = CardList.Num() - 1;
	for (int32 i = 0; i < NumShuffles; ++i)
	{
		int32 SwapIndex = FMath::RandRange(i, NumShuffles);
		CardList.Swap(i, SwapIndex);
	}

	for (int i = 0; i < CardList.Num(); ++i)
	{
		Cards.Push(std::move(CardList[i]));
	}
}

TArray<UCardDataRuntime*> UDeckObjectBase::GetCards(bool IncludeOutOfDeck, bool IncludeDiscard)
{
	TArray<UCardDataRuntime*> OutCards;

	for (auto Card : Cards)
	{
		OutCards.Add(Card);
	}

	if (IncludeOutOfDeck)
	{
		for (auto Card : OutOfDeckCards)
		{
			OutCards.Add(Card);
		}
	}

	if (IncludeDiscard && DiscardDeck)
	{
		for (auto Card : DiscardDeck->GetCards(false, false))
		{
			OutCards.Add(Card);
		}
	}

	return OutCards;
}
