// Copyright (c) 2025, Missing Planet LLC. All rights reserved.


#include "InventorySubsystem.h"

bool UInventorySubsystem::AddItemToInventory(FName ItemID, UDataTable* ItemSet)
{
	if (!ItemSet) return false;

	if (const auto Item = ItemSet->FindRow<FInventoryItem>(ItemID, "AddItem"))
	{
		InventoryItems.Add(*Item);
		return true;
	}

	return false;
}

bool UInventorySubsystem::AddExistingItemToInventory(const FInventoryItem& Item)
{
	InventoryItems.Add(Item);
	return true;
}

bool UInventorySubsystem::AddItemsToInventory(const TArray<FName>& ItemIDs, UDataTable* ItemSet)
{
	if (!ItemSet) return false;

	bool Result = true;
	for (const FName& ItemID : ItemIDs)
	{
		if (!AddItemToInventory(ItemID, ItemSet)) Result = false;
	}

	return Result;
}

bool UInventorySubsystem::RemoveItemFromInventory(FName ItemID)
{
	for (const FInventoryItem& Item : InventoryItems)
	{
		if (Item.ItemName == ItemID)
		{
			InventoryItems.RemoveSingle(Item);
			return true;
		}
	}

	return false;
}

bool UInventorySubsystem::RemoveItemsFromInventory(const TArray<FName>& ItemIDs)
{
	bool Result = true;

	for (const FName& ItemID : ItemIDs)
	{
		if (!RemoveItemFromInventory(ItemID)) Result = false;
	}

	return Result;
}
