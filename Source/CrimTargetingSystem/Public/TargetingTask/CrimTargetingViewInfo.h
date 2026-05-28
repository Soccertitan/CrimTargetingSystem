// Copyright Soccertitan 2026

#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "CrimTargetingViewInfo.generated.h"

struct FTargetingRequestHandle;

/**
 * Retrieves the ViewInfo from a TargetingContext.
 */
UCLASS(DefaultToInstanced, BlueprintType)
class CRIMTARGETINGSYSTEM_API UCrimTargetingViewInfo : public UObject
{
	GENERATED_BODY()
	
public:
	/** Retrieves the view from the SourceActor if valid, otherwise will try and grab it from the Instigator. */
	virtual void GetViewInfo(const FTargetingRequestHandle& TargetingHandle, FMinimalViewInfo& OutResult) const;
	
protected:
	/** If false, will use the Instigator's view at a higher priority. */
	UPROPERTY(EditAnywhere, Category = "Target View")
	bool bPrioritizeSourceActor = true;
	
	/** If true, uses the Camera's Eyes function instead of the CalcCamera function. */
	UPROPERTY(EditAnywhere, Category = "Target View")
	bool bUseActorEyes = false;
	
	/** Will use this ViewInfo if a camera is not found. */
	UPROPERTY(EditAnywhere, Category = "Target View", AdvancedDisplay)
	FMinimalViewInfo DefaultViewInfo;
};
