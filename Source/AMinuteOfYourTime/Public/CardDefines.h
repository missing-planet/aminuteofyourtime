#pragma once

#include "CoreMinimal.h"
#include "CardDefines.generated.h"

UENUM(BlueprintType)
enum ECard : uint8
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

	FCardAbilityData(TArray<TEnumAsByte<ECard>> Cards, FText Name, FText Description,
		TEnumAsByte<EAbilityType> Type)
			: AssociatedCards(Cards)
			, AbilityName(Name)
			, AbilityType(Type)
	{}

	UPROPERTY(Blueprintable, BlueprintReadWrite)
	TArray<TEnumAsByte<ECard>> AssociatedCards;
	
	UPROPERTY(Blueprintable, BlueprintReadOnly)
	FText AbilityName = FText::FromString("Default Ability");

	UPROPERTY(Blueprintable, BlueprintReadOnly, meta = (MultiLine))
	FText AbilityDescription = FText::FromString("This is the default ability type.");

	UPROPERTY(Blueprintable, BlueprintReadOnly)
	TEnumAsByte<EAbilityType> AbilityType;

	UPROPERTY(Blueprintable, BlueprintReadOnly)
	FSlateBrush AbilityBrush;
};

extern static TArray<FCardAbilityData> GCardAbilityData;