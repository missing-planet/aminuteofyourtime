// Copyright (c) 2025, Missing Planet LLC. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "CardDataBase.h"
#include "CardHandleMethod.h"
#include "CardInterface.h"
#include "Blueprint/UserWidget.h"
#include "GameFramework/PlayerState.h"
#include "CardPlayerState.generated.h"

UDELEGATE(BlueprintCallable)
DECLARE_DYNAMIC_MULTICAST_DELEGATE_FiveParams(FHandChangedSignature, const TArray<UCardDataRuntime*>&, Hand,
	FVector2D, DrawLocation, int32, HandDelta, float, Delay, bool, Animate);

UDELEGATE(BlueprintCallable)
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FActionPointsChangedSignature, int32, ActionPoints);

UCLASS()
class AMINUTEOFYOURTIME_API ACardPlayerState : public APlayerState, public IPlayerHandInterface
{
	GENERATED_BODY()

public:

	UPROPERTY(BlueprintReadOnly)
	UDeckObjectBase* PlayerDeck;

	UPROPERTY(BlueprintReadWrite)
	TArray<UCardDataRuntime*> PlayerHand;

	UPROPERTY(BlueprintReadWrite)
	int32 MaxHandSize = 999;

	UPROPERTY(BlueprintReadWrite)
	int32 ActionPoints = 10;

	UPROPERTY(BlueprintReadWrite)
	int32 MaxActionPoints = 10;

	UPROPERTY(BlueprintAssignable, BlueprintCallable)
	FHandChangedSignature HandChangedEvent;

	UPROPERTY(BlueprintAssignable, BlueprintCallable)
	FActionPointsChangedSignature ActionPointsChangedEvent;

public:

	UFUNCTION(BlueprintCallable)
	void AddActionPoints(int32 Amount);

	UFUNCTION(BlueprintCallable)
	void SetActionPoints(int32 Amount);

	UFUNCTION(BlueprintCallable)
	void SetDeck(UDeckObjectBase* Deck) { PlayerDeck = Deck; }

	UFUNCTION(BlueprintCallable, BlueprintPure)
	UDeckObjectBase* GetDeck() const { return PlayerDeck; }

	UFUNCTION(BlueprintCallable)
	static void HandleSinkForSpendCard(UUserWidget* UserWidget, ECardHandleMethod HandleMethod, float Delay)
	{
		if (!UserWidget) return;

		switch (HandleMethod)
		{
		case ECardHandleMethod::Spend:
			UserWidget->RemoveFromParent();
			break;
		case ECardHandleMethod::Destroy:
			FTimerHandle Handle;
			UserWidget->GetWorld()->GetTimerManager().SetTimer(Handle, [UserWidget]()
			{
				UserWidget->RemoveFromParent();
			}, Delay, false);
			break;
		}
	}

	virtual void AddCard_Implementation(UCardDataRuntime* Card, FVector2D DrawLocation, float Delay,
		bool Broadcast = true, bool Animate = true) override;
	virtual void RemoveCard_Implementation(UCardDataRuntime* Card, bool Broadcast = true) override;
	virtual void AddCards_Implementation(const TArray<UCardDataRuntime*>& Cards, FVector2D DrawLocation, float Delay,
		bool Broadcast = true, bool Animate = true) override;
};
