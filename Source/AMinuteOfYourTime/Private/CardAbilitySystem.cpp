// Copyright (c) 2025, Missing Planet LLC. All rights reserved.


#include "CardAbilitySystem.h"

#include "Kismet/GameplayStatics.h"

const TArray<FCardAbilityData> GCardAbilityData = {
	{
		{
			ECard::Heart	
		},
		FText::FromString("Heart"),
		FText::FromString("Add 2 Muscle, 2 Blood, 2 Bone, and 2 Flesh to your deck and Refresh."),
		0,
		0,
		EAbilityType::Ritual
	},
	{
		{
			ECard::Bone
		},
		FText::FromString("Bone"),
		FText::FromString("Offer another Card with this one, decrease the Destroy chance of that Card by 10%."),
		1,
		1,
		EAbilityType::Ritual
	},
	{
		{
			ECard::Foot
		},
		FText::FromString("Foot"),
		FText::FromString("Increase AP Pool by 2"),
		0,
		0,
		EAbilityType::Ritual
	},

	{
		{
			ECard::Nose
		},
		FText::FromString("Nose"),
		FText::FromString("Destroys a basic card, Increase ??? by 15%"),
		0,
		0,
		EAbilityType::Ritual
	},

	{
		{
			ECard::BrianMatters
		},
		FText::FromString("Brain"),
		FText::FromString("Double your AP Pool for the day."),
		0,
		0,
		EAbilityType::Ritual

	}


};

UCardAbilitySystem* UCardAbilitySystem::GetCardAbilitySystem(UObject* WorldContext)
{
	if (UCardAbilitySystem* CardAbilitySystem =
		UGameplayStatics::GetGameInstance(WorldContext)->GetSubsystem<UCardAbilitySystem>())
	{
		return CardAbilitySystem;
	}

	return nullptr;
}

bool UCardAbilitySystem::DoesCardHaveAbilityOfType(ECard Card, EAbilityType Type) const
{
	if (Card == ECard::InvalidCard || Type == EAbilityType::InvalidAbility) return false;
	
	for (const FCardAbilityData& Ability : GCardAbilityData)
	{
		if (Ability.AbilityType == Type && Ability.AssociatedCards.Contains(Card))
			return true;
	}
	
	return false;
}

const TArray<FCardAbilityData>& UCardAbilitySystem::GetCardAbilityData() const
{
	return GCardAbilityData;
}

FVector2D UCardAbilitySystem::GetAbilityMinMaxOfferings(ECard Card) const
{
	for (const FCardAbilityData& Ability : GCardAbilityData)
	{
		if (Ability.AbilityType == EAbilityType::Ritual && Ability.AssociatedCards.Contains(Card))
		{
			return FVector2D(Ability.MinOfferingsForAbility, Ability.MaxOfferingsForAbility);
		}
	}

	return FVector2D(0, 0);
}
