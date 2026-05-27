// Copyright Soccertitan 2026

#pragma once

#include "CoreMinimal.h"

/** Context to search for a target. */
UENUM(BlueprintType)
enum class ETargetingSearchDirection : uint8
{
	Right,
	Left,
	Up,
	Down,
	Forward,
	Backward
};