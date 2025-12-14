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
			UCardDataBase* NewCard = NewObject<UCardDataBase>(this, UCardDataBase::StaticClass(),
			FName(*FString(Card->CardName.ToString() + "_" + Name.ToString() + "_" + FString::FromInt(i++))), RF_NoFlags, Card);
			NewCard->OwningDeck = this;
			Cards.Push(NewCard);
			CardCount++;
		}
		//NewCard->AddToRoot();
	}

	if (Shuffle) this->Shuffle();

	DeckCountChangeEvent.Broadcast(DeckData->CardList.Num());
}

int UDeckObjectBase::DrawCards(int Count, TArray<UCardDataBase*>& OutCardList, bool BroadcastChange)
{
	int ReturnCount = 0;

	for (int i = 0; i < Count; ++i)
	{
		if (Cards.IsEmpty())
		{
			if (!DiscardDeck) break;

			TArray<UCardDataBase*> DiscardedCards;
			int32 Amount = DiscardDeck->DrawCards(DiscardDeck->GetCardCount(), DiscardedCards, false);
			DiscardDeck->DeckCountChangeEvent.Broadcast(Amount);
			AddCards(DiscardedCards);
			Shuffle();
		}

		if (Cards.IsEmpty()) continue;
		
		TObjectPtr<UCardDataBase> Card = Cards.Pop();

		//Card->RemoveFromRoot();
		OutCardList.Add(Card);
		OutOfDeckCards.Add(Card);
		ReturnCount++;

		CardCount--;

		if (BroadcastChange) DeckCountChangeEvent.Broadcast(1);
	}

	return ReturnCount;
}

void UDeckObjectBase::AddCard(UCardDataBase* Card)
{
	if (!Card) return;
	
	Cards.Push(Card);
	//Card->AddToRoot();
	if (OutOfDeckCards.Contains(Card)) OutOfDeckCards.Remove(Card);

	CardCount++;
	DeckCountChangeEvent.Broadcast(1);
}

void UDeckObjectBase::AddCards(const TArray<UCardDataBase*>& CardList)
{
	for (UCardDataBase* card : CardList)
	{
		AddCard(card);
	}
}

void UDeckObjectBase::Shuffle()
{
	TArray<UCardDataBase*> CardList;

	while (!Cards.IsEmpty())
	{
		TObjectPtr<UCardDataBase> Card = Cards.Pop();
		//Card->RemoveFromRoot();
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
		//CardList[i]->AddToRoot();
	}
}
