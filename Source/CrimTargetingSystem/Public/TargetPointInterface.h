// Copyright Soccertitan 2026

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "TargetPointInterface.generated.h"

// This class does not need to be modified.
UINTERFACE()
class UTargetPointInterface : public UInterface
{
	GENERATED_BODY()
};

/**
 * Can be added to actors/components to determine if it can be targeted.
 */
class CRIMTARGETINGSYSTEM_API ITargetPointInterface
{
	GENERATED_BODY()

	// Add interface functions to this class. This is the class that will be inherited to implement this interface.
public:
	/** Returns true if the Controller can lock onto the point. */
	UFUNCTION(BlueprintNativeEvent)
	bool IsTargetPointTargetable(AController* SourceController);
};
