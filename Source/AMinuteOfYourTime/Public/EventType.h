#pragma once

UENUM(BlueprintType)
enum class EventType : uint8
{
	Lore,
	Exchange,
	Evidence,
	Clues,
	Other,
	
	Count UMETA(Hidden)
};