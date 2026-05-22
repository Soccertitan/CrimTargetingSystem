// Copyright Soccertitan 2026

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "LockOnPawnInterface.generated.h"

struct FCrimTargetPoint;
// This class does not need to be modified.
UINTERFACE()
class ULockOnPawnInterface : public UInterface
{
	GENERATED_BODY()
};

/**
 * An interface designed to be placed on a Pawn or a component on that pawn. The LockOnSystemComponent will signal
 * to the interface whenever the lock on target changes.
 */
class CRIMLOCKONSYSTEM_API ILockOnPawnInterface
{
	GENERATED_BODY()

	// Add interface functions to this class. This is the class that will be inherited to implement this interface.
public:
	UFUNCTION(BlueprintNativeEvent)
	void SetLockOnPoint(const FCrimTargetPoint& NewLockOnPoint);
};
