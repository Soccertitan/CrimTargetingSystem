// Copyright Soccertitan 2026

#pragma once

#include "CoreMinimal.h"
#include "CrimTargetingEnums.h"
#include "CrimViewTargetingTask.h"
#include "ScalableFloat.h"
#include "TargetingSelectionTask_FindTarget.generated.h"

/**
 * Find a target in the specified direction using the view. If the Context's SourceLocation is empty will select
 * the nearest actor.
 */
UCLASS()
class CRIMTARGETINGSYSTEM_API UTargetingSelectionTask_FindTarget : public UCrimViewTargetingTask
{
	GENERATED_BODY()
	
public:
	virtual void Execute(const FTargetingRequestHandle& TargetingHandle) const override;
	
	/** Selects the closest target based on the HitResult distance result. */
	void FindNearestTarget(const FTargetingRequestHandle& TargetingHandle) const;
	
	/** Selects the next target in the direction. */
	void FindTarget(const FTargetingRequestHandle& TargetingHandle) const;
	
protected:
	virtual ETargetingSearchDirection GetSearchDirection(const FTargetingRequestHandle& TargetingHandle) const;
	/** Uses the source location to start searching from that location. */
	virtual FVector GetOriginLocation(const FTargetingRequestHandle& TargetingHandle) const;
	
	/** Returns the normalized degree when searching in a direction. Clamped between 0.f and 1.f */
	virtual float GetDotProductTargetThreshold(const FTargetingRequestHandle& TargetingHandle) const;
	
	/** Gets the appropriate direction vector from the ViewInfo rotation for the search. */
	static FVector GetSearchDirectionVector(ETargetingSearchDirection SearchDirection, const FMinimalViewInfo& ViewInfo);
	
private:
	/** The direction to search for targets. */
	UPROPERTY(EditAnywhere, Category = "Target Selection Find Target", DisplayName = "SearchDirection")
	ETargetingSearchDirection DefaultSearchDirection = ETargetingSearchDirection::Right;
	
	/** The normalized degree to which the target must be to the positive or negative of the origin point to be targetable expects a value of range of 0-1. */
	UPROPERTY(EditAnywhere, Category = "Target Selection Find Target", meta = (ClampMin = 0, ClampMax = 1), DisplayName = "DotProductTargetThreshold")
	FScalableFloat DefaultDotProductTargetThreshold = 0.f;
};
