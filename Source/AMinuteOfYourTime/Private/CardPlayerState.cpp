// Copyright (c) 2025, Missing Planet LLC. All rights reserved.


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

void ACardPlayerState::AddCard_Implementation(UCardDataRuntime* Card, FVector2D DrawLocation, float Delay, bool Broadcast)
{
	if (PlayerHand.Num() >= MaxHandSize) return;

	PlayerHand.Add(Card);

	if (Broadcast)
		HandChangedEvent.Broadcast(PlayerHand, DrawLocation, 1, Delay);
}

void ACardPlayerState::RemoveCard_Implementation(UCardDataRuntime* Card, bool Broadcast)
{
	if (!PlayerHand.Contains(Card)) return;

	PlayerHand.RemoveSingle(Card);

	if (Broadcast)
		HandChangedEvent.Broadcast(PlayerHand, FVector2D(999, 999), -1, 0);
}

void ACardPlayerState::AddCards_Implementation(const TArray<UCardDataRuntime*>& Cards, FVector2D DrawLocation,
	float Delay, bool Broadcast)
{
	if (PlayerHand.Num() >= MaxHandSize) return;

	int32 MaxDrawCount = std::min(Cards.Num(), MaxHandSize - PlayerHand.Num());

	if (MaxDrawCount <= 0) return;

	for (int32 i = 0; i < MaxDrawCount; ++i)
	{
		PlayerHand.Add(Cards[i]);
	}
	// TODO: Something needs to be done with the excess cards if we try to add more than MaxDrawCount

	if (Broadcast)
		HandChangedEvent.Broadcast(PlayerHand, DrawLocation, MaxDrawCount, Delay);
}
