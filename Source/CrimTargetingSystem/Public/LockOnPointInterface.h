// Copyright Soccertitan 2026

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "LockOnPointInterface.generated.h"

// This class does not need to be modified.
UINTERFACE()
class ULockOnPointInterface : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class CRIMLOCKONSYSTEM_API ILockOnPointInterface
{
	GENERATED_BODY()

	// Add interface functions to this class. This is the class that will be inherited to implement this interface.
public:
	/** Returns true if the PlayerController can lock onto the point. */
	UFUNCTION(BlueprintNativeEvent)
	bool IsLockOnPointTargetable(APlayerController* SourcePlayerController);
};
