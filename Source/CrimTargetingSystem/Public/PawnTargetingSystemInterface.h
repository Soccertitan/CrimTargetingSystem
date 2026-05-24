// Copyright Soccertitan 2026

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "PawnTargetingSystemInterface.generated.h"

struct FCrimTargetPoint;
// This class does not need to be modified.
UINTERFACE()
class UPawnTargetingSystemInterface : public UInterface
{
	GENERATED_BODY()
};

/**
 * An interface designed to be placed on a Pawn or a component on that pawn. The TargetingSystemComp will call these
 * functions on the first component that implements this Interface and falls back to the Controlled Pawn.
 */
class CRIMTARGETINGSYSTEM_API IPawnTargetingSystemInterface
{
	GENERATED_BODY()

	// Add interface functions to this class. This is the class that will be inherited to implement this interface.
public:
	UFUNCTION(BlueprintNativeEvent)
	void SetTargetPoint(const FCrimTargetPoint& NewTargetPoint);
	UFUNCTION(BlueprintNativeEvent)
	void SetLockOnState(const bool bEnabled);
};
