// Copyright Soccertitan 2026

#pragma once

#include "CoreMinimal.h"
#include "Tasks/TargetingTask.h"
#include "CrimViewTargetingTask.generated.h"

/**
 * Utility functions to retrieve the view.
 */
UCLASS(Abstract)
class CRIMTARGETINGSYSTEM_API UCrimViewTargetingTask : public UTargetingTask
{
	GENERATED_BODY()
	
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
	
	/** Retrieves the view from the SourceActor if valid, otherwise will try and grab it from the Instigator. */
	virtual void GetViewInfo(const FTargetingRequestHandle& TargetingHandle, FMinimalViewInfo& OutResult) const;
};
