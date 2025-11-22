// Copyright (c) 2025, Missing Planet LLC. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Runtime/Engine/Classes/Engine/DataTable.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "InventorySubsystem.generated.h"

USTRUCT(BlueprintType)
struct FInventoryItem : public FTableRowBase 
{
	GENERATED_USTRUCT_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FName ItemName;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FText ItemDescription;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UMaterialInterface* Material = nullptr;

	bool operator==(const FInventoryItem& other) const
	{
		return other.ItemName == ItemName;
	}
};

UCLASS()
class AMINUTEOFYOURTIME_API UInventorySubsystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()

public:

	UFUNCTION(BlueprintCallable)
	bool AddItemToInventory(FName ItemID, UDataTable* ItemSet);

	UFUNCTION(BlueprintCallable)
	bool AddItemsToInventory(const TArray<FName>& ItemIDs, UDataTable* ItemSet);

	UFUNCTION(BlueprintCallable)
	bool RemoveItemFromInventory(FName ItemID);

	UFUNCTION(BlueprintCallable)
	bool RemoveItemsFromInventory(const TArray<FName>& ItemIDs);

	UFUNCTION(BlueprintCallable)
	const TArray<FInventoryItem>& GetInventoryItems() const { return InventoryItems; }

	UFUNCTION(BlueprintCallable)
	int32 GetInventorySize() const { return InventoryItems.Num(); }

private:

	UPROPERTY()
	TArray<FInventoryItem> InventoryItems;
};
