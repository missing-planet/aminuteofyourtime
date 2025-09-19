// Fill out your copyright notice in the Description page of Project Settings.

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

	// Returns actual number of cards drawn, in the case that the deck has less than Count
	UFUNCTION(BlueprintCallable)
	int DrawCards(int Count, TArray<UCardDataBase*>& OutCardList, bool BroadcastChange = false);

	UFUNCTION(BlueprintCallable)
	void AddCard(UCardDataBase* Card);

	UFUNCTION(BlueprintCallable)
	void AddCards(const TArray<UCardDataBase*>& CardList);

	UFUNCTION(BlueprintCallable)
	void Shuffle();

	UFUNCTION(BlueprintCallable)
	void LockDeck(bool Lock) { bDeckLocked = Lock; }

	UFUNCTION(BlueprintCallable)
	bool IsDeckLocked() const { return bDeckLocked; }

private:

	int32 CardCount = 0;

	bool bDeckLocked = false;

	TQueue<UCardDataBase*> Cards;
};
