// Copyright (c) 2025, Missing Planet LLC. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "CardDefines.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "CardAbilitySystem.generated.h"

UCLASS(BlueprintType)
class AMINUTEOFYOURTIME_API UCardAbilitySystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()

public:

	UFUNCTION(BlueprintCallable, BlueprintPure)
	static UCardAbilitySystem* GetCardAbilitySystem(UObject* WorldContext);

	UFUNCTION(BlueprintCallable, BlueprintPure)
	bool DoesCardHaveAbilityOfType(ECard Card, EAbilityType Type) const;

	UFUNCTION(BlueprintCallable, BlueprintPure)
	const TArray<FCardAbilityData>& GetCardAbilityData() const;

public:

	UPROPERTY(BlueprintReadWrite)
	int32 NumPerformedRituals = 0;
	
	UPROPERTY(BlueprintReadWrite)
	int32 MaxRituals = 2;
};