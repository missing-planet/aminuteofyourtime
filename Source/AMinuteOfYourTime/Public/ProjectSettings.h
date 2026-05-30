#pragma once

#include "CoreMinimal.h"
#include "Engine/DeveloperSettings.h"
#include "ProjectSettings.generated.h"

UCLASS(Config=Game, defaultconfig, meta=(DisplayName="Location System Settings"))
class AMINUTEOFYOURTIME_API ULocationProjectSettings : public UDeveloperSettings
{
	GENERATED_BODY()

public:
	ULocationProjectSettings()
	{
		// Points to the category section under Project Settings
		CategoryName = TEXT("Game"); 
	}

	// This creates an explicit asset picker inside Edit -> Project Settings
	UPROPERTY(Config, EditAnywhere, BlueprintReadOnly, Category = "Probabilities")
	TSoftObjectPtr<UDefaultProbabilitiesDataAsset> DefaultProbabilityDataAsset;

	UPROPERTY(Config, EditAnywhere, BlueprintReadOnly, Category = "Events")
	TMap<ELocation, TSoftObjectPtr<UDataTable>> LocationEvents;
};