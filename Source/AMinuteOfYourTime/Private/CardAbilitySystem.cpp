// Fill out your copyright notice in the Description page of Project Settings.


#include "CardAbilitySystem.h"

static TArray<FCardAbilityData> GCardAbilityData = {
	{
		{
			ECard::Heart	
		},
		FText::FromString("Heart"),
		FText::FromString("Just an organ that pumps blood."),
		EAbilityType::Ritual
	},
};