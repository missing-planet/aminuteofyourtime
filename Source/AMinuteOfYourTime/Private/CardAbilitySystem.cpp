// Copyright (c) 2025, Missing Planet LLC. All rights reserved.


#include "CardAbilitySystem.h"

#include "Kismet/GameplayStatics.h"

const TArray<FCardAbilityData> GCardAbilityData = {
	{
		{
			ECard::Heart	
		},
		FText::FromString("Heart"),
		FText::FromString("Just an organ that pumps blood."),
		EAbilityType::Ritual
	},
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
