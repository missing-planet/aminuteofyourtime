// Copyright (c) 2025, Missing Planet LLC. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "CardDefines.generated.h"

UENUM(BlueprintType)
enum class ECard : uint8
{
	InvalidCard = 0,  

	FirstCard							UMETA(DisplayName = "Blood"),
	
	FirstBasicCard = FirstCard			UMETA(Hidden),
	Blood = FirstBasicCard				UMETA(Hidden),
	Bone,
	Flesh,
	Muscle,
	LastBasicCard = Muscle				UMETA(Hidden),

	FirstAppendage						UMETA(DisplayName = "Arm"),
	Arm = FirstAppendage				UMETA(Hidden),
	Hand,
	Finger,
	Leg,
	Foot,
	Toe,
	LastAppendage = Toe					UMETA(Hidden),

	FirstSense							UMETA(DisplayName = "Eye"),
	Eye = FirstSense					UMETA(Hidden),
	Nose,
	Ear,
	Mouth,
	LastSense = Mouth					UMETA(Hidden),

	FirstOrgan							UMETA(DisplayName = "Heart"),
	Heart = FirstOrgan					UMETA(Hidden),
	BrianMatters						UMETA(DisplayName = "Brain"),						
	LastOrgan = BrianMatters			UMETA(Hidden),

	LastCard = LastOrgan				UMETA(Hidden),

	NumCards = LastCard
};

UENUM(BlueprintType)
enum EAbilityType
{
	InvalidAbility = 0,

	FirstAbilityType					UMETA(DisplayName = "Passive"),
	Passive = FirstAbilityType			UMETA(Hidden),
	CardModifier,
	Sacrifice,
	Ritual,
	LastAbilityType = Ritual			UMETA(Hidden),

	NumAbilityTypes = LastAbilityType
};

USTRUCT(BlueprintType)
struct FCardAbilityData
{
	GENERATED_BODY()

	FCardAbilityData() = default;
	FCardAbilityData(const TArray<ECard>& Cards, const FText& Name,
		const FText& Description, int32 MinOfferings, int32 MaxOfferings, TEnumAsByte<EAbilityType> Type)
			: AssociatedCards(Cards)
			, AbilityName(Name)
			, AbilityDescription(Description)
			, MinOfferingsForAbility(MinOfferings)
			, MaxOfferingsForAbility(MaxOfferings)
			, AbilityType(Type)
	{}

	UPROPERTY(Blueprintable, BlueprintReadWrite)
	TArray<ECard> AssociatedCards;
	
	UPROPERTY(Blueprintable, BlueprintReadOnly)
	FText AbilityName = FText::FromString("Default Ability");

	UPROPERTY(Blueprintable, BlueprintReadOnly, meta = (MultiLine))
	FText AbilityDescription = FText::FromString("This is the default ability type.");

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	int32 MinOfferingsForAbility = 1;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	int32 MaxOfferingsForAbility = 3;

	UPROPERTY(Blueprintable, BlueprintReadOnly)
	TEnumAsByte<EAbilityType> AbilityType = TEnumAsByte<EAbilityType>(0);

	UPROPERTY(Blueprintable, BlueprintReadOnly)
	FSlateBrush AbilityBrush;
};

extern const TArray<FCardAbilityData> GCardAbilityData;