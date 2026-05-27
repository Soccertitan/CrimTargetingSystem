// Copyright Soccertitan 2026

#pragma once

#include "CoreMinimal.h"
#include "ScalableFloat.h"
#include "Tasks/TargetingTask.h"
#include "CrimViewTargetingTask.generated.h"

/**
 * Utility functions to retrieve the view.
 */
UCLASS(Abstract)
class CRIMTARGETINGSYSTEM_API UCrimViewTargetingTask : public UTargetingTask
{
	GENERATED_BODY()
	
public:
	/** If false, will use the Instigator's view at a higher priority. */
	UPROPERTY(EditAnywhere, Category = "Data")
	bool bPrioritizeSourceActor = true;
	
	/** If true, uses the Camera's Eyes function instead of the CalcCamera function. */
	UPROPERTY(EditAnywhere, Category = "Data")
	bool bUseActorEyes = false;
	
	/** If using the Actor's eyes, specify the angle for the cone. */
	UPROPERTY(EditAnywhere, Category = "Data", meta = (EditCondition="bUseActorEyes"))
	FScalableFloat FieldOfView = 180.f;
	
protected:
	/** Retrieves the view from the SourceActor if valid, otherwise will try and grab it from the Instigator. */
	virtual void GetViewInfo(const FTargetingRequestHandle& TargetingHandle, FMinimalViewInfo& OutResult) const;
};
