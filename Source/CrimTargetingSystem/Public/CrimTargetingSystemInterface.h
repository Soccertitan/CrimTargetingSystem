// Copyright Soccertitan 2026

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "CrimTargetingSystemInterface.generated.h"

class UCrimTargetingSystemComponent;
// This class does not need to be modified.
UINTERFACE()
class UCrimTargetingSystemInterface : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class CRIMTARGETINGSYSTEM_API ICrimTargetingSystemInterface
{
	GENERATED_BODY()

	// Add interface functions to this class. This is the class that will be inherited to implement this interface.
public:
	UFUNCTION(BlueprintNativeEvent)
	UCrimTargetingSystemComponent* GetCrimTargetingSystemComponent() const;
};
