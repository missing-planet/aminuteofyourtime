// Fill out your copyright notice in the Description page of Project Settings.


#include "CardPlayerState.h"

void ACardPlayerState::AddActionPoints(int32 Amount)
{
	ActionPoints = FMath::Clamp(ActionPoints + Amount, 0, MaxActionPoints);

	ActionPointsChangedEvent.Broadcast(ActionPoints);
}

void ACardPlayerState::SetActionPoints(int32 Amount)
{
	ActionPoints = FMath::Clamp(Amount, 0, MaxActionPoints);

	ActionPointsChangedEvent.Broadcast(ActionPoints);
}

void ACardPlayerState::AddCard_Implementation(UCardDataBase* Card, FVector2D DrawLocation)
{
	if (PlayerHand.Num() >= MaxHandSize) return;

	PlayerHand.Add(Card);

	HandChangedEvent.Broadcast(PlayerHand, DrawLocation, 1);
}

void ACardPlayerState::RemoveCard_Implementation(UCardDataBase* Card)
{
	if (!PlayerHand.Contains(Card)) return;

	PlayerHand.RemoveSingle(Card);

	HandChangedEvent.Broadcast(PlayerHand, FVector2D(999, 999), -1);
}

void ACardPlayerState::AddCards_Implementation(const TArray<UCardDataBase*>& Cards, FVector2D DrawLocation)
{
	if (PlayerHand.Num() >= MaxHandSize) return;

	int32 MaxDrawCount = Cards.Num() - PlayerHand.Num();

	if (MaxDrawCount <= 0) return;

	for (int32 i = 0; i < MaxDrawCount; ++i)
	{
		PlayerHand.Add(Cards[i]);

		HandChangedEvent.Broadcast(PlayerHand, DrawLocation, MaxDrawCount);
	}
}
