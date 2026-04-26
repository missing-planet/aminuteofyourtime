// Copyright (c) 2025, Missing Planet LLC. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "DeckDataBase.h"
#include "UObject/NoExportTypes.h"
#include "DeckObjectBase.generated.h"

UDELEGATE(BlueprintCallable)
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FDeckCountChangeSignature, int32, NewCount);

UCLASS(BlueprintType, DisplayName="Deck Object")
class AMINUTEOFYOURTIME_API UDeckObjectBase : public UObject
{
	GENERATED_BODY()

public:

	UPROPERTY(BlueprintReadOnly)
	FName DeckName = "Empty Deck";

	UPROPERTY(BlueprintAssignable, BlueprintCallable)
	FDeckCountChangeSignature DeckCountChangeEvent;

public:

	UFUNCTION(BlueprintCallable)
	void Initialize(UDeckDataBase* DeckData, FName Name, bool Shuffle);

	UFUNCTION(BlueprintCallable)
	int32 GetCardCount() const { return CardCount; }

	UFUNCTION(BlueprintCallable)
	int32 GetTotalCardCount() const { return CardCount + OutOfDeckCards.Num(); }

	UFUNCTION(BlueprintCallable)
	void SetDiscardDeck(UDeckObjectBase* Discard) { DiscardDeck = Discard; }

	UFUNCTION(BlueprintCallable)
	UDeckObjectBase* GetDiscardDeck() const { return DiscardDeck; }

	// Returns actual number of cards drawn, in the case that the deck has less than Count
	UFUNCTION(BlueprintCallable)
	int DrawCards(int Count, TArray<UCardDataRuntime*>& OutCardList, bool BroadcastChange = false);

	UFUNCTION(BlueprintCallable)
	void AddCard(UCardDataRuntime* Card);

	UFUNCTION(BlueprintCallable)
	void AddCards(const TArray<UCardDataRuntime*>& CardList);

	UFUNCTION(BlueprintCallable)
	void RemoveCard(UCardDataRuntime* Card);

	UFUNCTION(BlueprintCallable)
	void RemoveCards(const TArray<UCardDataRuntime*>& CardList);

	UFUNCTION(BlueprintCallable)
	void Shuffle();

	UFUNCTION(BlueprintCallable)
	void LockDeck(bool Lock) { bDeckLocked = Lock; }

	UFUNCTION(BlueprintCallable)
	bool IsDeckLocked() const { return bDeckLocked; }

	UFUNCTION(BlueprintCallable)
	TArray<UCardDataRuntime*> GetCards(bool IncludeOutOfDeck, bool IncludeDiscard);

private:

	int32 CardCount = 0;

	bool bDeckLocked = false;
	
	UPROPERTY()
	TArray<TObjectPtr<UCardDataRuntime>> Cards;

	UPROPERTY()
	TArray<TObjectPtr<UCardDataRuntime>> OutOfDeckCards;

	UPROPERTY()
	TObjectPtr<UDeckObjectBase> DiscardDeck;
};
