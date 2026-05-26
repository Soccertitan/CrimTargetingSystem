// Copyright Soccertitan 2026

#pragma once

#include "CoreMinimal.h"
#include "Tasks/TargetingTask.h"
#include "TargetingSelectionTask_FindNextTarget.generated.h"

/**
 * Find a target in the specified direction using the SourceActor's camera view for direction. If SourceLocation is empty,
 * find the nearest target, otherwise find the next target in the specified direction.
 */
UCLASS()
class CRIMTARGETINGSYSTEM_API UTargetingSelectionTask_FindNextTarget : public UTargetingTask
{
	GENERATED_BODY()
	
public:
	virtual void Execute(const FTargetingRequestHandle& TargetingHandle) const override;
	
	/** Selects the closest target based on the HitResult distance. */
	void FindNearestTarget(const FTargetingRequestHandle& TargetingHandle) const;
	/** Selects the next target in the direction. */
	void FindNextTarget(const FTargetingRequestHandle& TargetingHandle) const;
	
protected:
	virtual EUINavigation GetSearchDirection(const FTargetingRequestHandle& TargetingHandle) const;
	/** The starting location to start a search from. */
	virtual FVector GetOriginLocation(const FTargetingRequestHandle& TargetingHandle) const;
	/** The location that is the central when searching right/left/up/down from. */
	virtual void GetViewInfo(const FTargetingRequestHandle& TargetingHandle, FMinimalViewInfo& OutResult) const;
	
private:
	/** The direction to search for targets. */
	UPROPERTY(EditAnywhere, Category = "Data", meta = (ValidEnumValues="Left,Right,Up,Down"), DisplayName = "SearchDirection")
	EUINavigation DefaultSearchDirection = EUINavigation::Right;
	
	/** If true, allows the selection of a target that is behind the view. */
	UPROPERTY(EditAnywhere, Category = "Data")
	bool bTargetMustBeInView = false;
	
	/** The normalized degree to which the target must be to the right or left of the origin point to be targetable. */
	UPROPERTY(EditAnywhere, Category = "Data", meta = (ClampMin = 0, ClampMax = 1))
	float RightLeftTargetThreshold = 0.f;
	
	/** The normalized degree to which the target must be above or below of the origin point to be targetable. */
	UPROPERTY(EditAnywhere, Category = "Data", meta = (ClampMin = 0, ClampMax = 1))
	float UpDownTargetThreshold = 0.2f;
};
