// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "CardDefines.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "CardAbilitySystem.generated.h"

/**
 * 
 */
UCLASS()
class AMINUTEOFYOURTIME_API UCardAbilitySystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()

public:

	UFUNCTION(BlueprintCallable, BlueprintPure)
	bool DoesCardHaveAbilityOfType(ECard Card, EAbilityType Type);
};
