// Copyright Soccertitan 2026

#pragma once

#include "CoreMinimal.h"
#include "Tasks/TargetingTask.h"
#include "TargetingSelectionTask_FindTarget.generated.h"


/**
 * Find a target in the specified direction using the view. If the Context's SourceLocation is empty will select
 * the nearest actor.
 */
UCLASS()
class CRIMTARGETINGSYSTEM_API UTargetingSelectionTask_FindTarget : public UTargetingTask
{
	GENERATED_BODY()
	
public:
	UTargetingSelectionTask_FindTarget(const FObjectInitializer& ObjectInitializer);
	
	virtual void Execute(const FTargetingRequestHandle& TargetingHandle) const override;
	
	/** Selects the closest target based on the HitResult distance result. */
	void FindNearestTarget(const FTargetingRequestHandle& TargetingHandle) const;
	
	/** Selects the next target in the direction. */
	void FindTarget(const FTargetingRequestHandle& TargetingHandle) const;
	
protected:
	/** Returns the default search direction. */
	virtual FVector2D GetSearchDirection(const FTargetingRequestHandle& TargetingHandle) const;
	
	/** Uses the source location to start searching from that location. */
	virtual FVector GetOriginLocation(const FTargetingRequestHandle& TargetingHandle) const;
	
	/** Retrieves the view from the SourceActor if valid, otherwise will try and grab it from the Instigator. */
	virtual void GetViewInfo(const FTargetingRequestHandle& TargetingHandle, FMinimalViewInfo& OutResult) const;
	
	/** Returns the default max search angle. */
	virtual float GetSearchAngle(const FTargetingRequestHandle& TargetingHandle) const;
	
	/** Returns the default wrap search. */
	virtual bool GetWrapSearch(const FTargetingRequestHandle& TargetingHandle) const;
	
	/** 
	 * Returns true if the target should be considered a valid target. 
	 * Checks if HitResult is within view, does a line trace to check for a blocked hit, and returns the 2d screen position.
	 */
	virtual bool IsValidTarget(const FTargetingRequestHandle& TargetingHandle, const FMinimalViewInfo& ViewInfo, 
		const FMatrix& ViewProjectionMatrix, const FHitResult& HitResult, FVector2D& OutScreenPos) const;
	
	/** 
	 * Returns a score for the target. The FindNextTarget function will take this value and act according to these rules. 
	 * 1. If the score is == 0.f, do nothing.
	 * 2. The score is greater than 0, it will use the smallest value that is returned.
	 * 3. If the score is less than 0, it will use the greatest value that is returned when wrapping is enabled.
	 * 
	 * The Direction is expected to be normalized.
	 */
	virtual float CalculateTargetScore(const FTargetingRequestHandle& TargetingHandle, const FVector2D& Origin, const FVector2D& Target, const FVector2D& Direction, const float SearchAngle) const;
	
private:
	/** The direction to search for targets. X = 1 is to the right. And Y = 1 is Up. */
	UPROPERTY(EditAnywhere, Category = "Find Target", DisplayName = "SearchDirection", meta  = (ClampMin = -1, ClampMax = 1))
	FVector2D DefaultSearchDirection = {1.f, 0.f};

	/** The target must be within (inclusive) this angle of the current target to be selected. 90 degrees is perpendicular. */
	UPROPERTY(EditAnywhere, Category = "Find Target", DisplayName = "MaxSearchAngle", meta = (ClampMin = 0, ClampMax = 180, Units = "Degrees"))
	float DefaultSearchAngle = 90.f;
	
	/** If True, will include search results that are in the opposite search direction. */
	UPROPERTY(EditAnywhere, Category = "Find Target", DisplayName = "WrapSearch")
	bool bDefaultWrapSearch = true;
	
	/** Will use this ViewInfo if a camera is not found. */
	UPROPERTY(EditAnywhere, Category = "Find Target|View", AdvancedDisplay)
	FMinimalViewInfo DefaultViewInfo;
	
	/** The trace channel to use */
	UPROPERTY(EditAnywhere, Category = "Find Target|Trace")
	TEnumAsByte<ETraceTypeQuery> TraceChannel;
	
	/** Indicates the trace should perform a complex trace */
	UPROPERTY(EditAnywhere, Category = "Find Target|Trace")
	uint8 bComplexTrace : 1;
	
	bool IsViewBlocked(const FTargetingRequestHandle& TargetingHandle, const FVector& Start, const FVector& End) const;
	
	/** Setup CollisionQueryParams for the trace */
	void InitCollisionParams(const FTargetingRequestHandle& TargetingHandle, FCollisionQueryParams& OutParams) const;
};
