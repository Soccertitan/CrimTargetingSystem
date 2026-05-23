// Copyright Soccertitan 2026

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "PawnTargetPointInterface.generated.h"

struct FCrimTargetPoint;
// This class does not need to be modified.
UINTERFACE()
class UPawnTargetPointInterface : public UInterface
{
	GENERATED_BODY()
};

/**
 * An interface designed to be placed on a Pawn or a component on that pawn. The PlayerLockOnComponent will inform
 * the interface whenever the locked on target changes.
 */
class CRIMTARGETINGSYSTEM_API IPawnTargetPointInterface
{
	GENERATED_BODY()

	// Add interface functions to this class. This is the class that will be inherited to implement this interface.
public:
	UFUNCTION(BlueprintNativeEvent)
	void SetTargetPoint(const FCrimTargetPoint& NewTargetPoint);
};
