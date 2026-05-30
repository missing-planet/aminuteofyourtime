#pragma once

UENUM(BlueprintType)
enum class EventType : uint8
{
	Lore,
	Exchange,
	Clue,
	Other UMETA(DisplayName = "???"),
	
	Count UMETA(Hidden)
};