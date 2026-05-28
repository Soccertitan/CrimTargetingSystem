// Copyright Soccertitan 2026

#pragma once

#include "CoreMinimal.h"
#include "CrimViewTargetingTask.h"
#include "TargetingFilterTask_IsViewBlocked.generated.h"

/**
 * Performs a line trace to each HitResult using the Actor's view. If there is a blocking hit, filters out the HitResult.
 */
UCLASS()
class CRIMTARGETINGSYSTEM_API UTargetingFilterTask_IsViewBlocked : public UCrimViewTargetingTask
{
	GENERATED_BODY()
	
public:
	UTargetingFilterTask_IsViewBlocked(const FObjectInitializer& ObjectInitializer);
	
	virtual void Execute(const FTargetingRequestHandle& TargetingHandle) const override;

protected:
	/** The trace channel to use */
	UPROPERTY(EditAnywhere, Category = "Target Filter View")
	TEnumAsByte<ETraceTypeQuery> TraceChannel;
	
	/** Indicates the trace should perform a complex trace */
	UPROPERTY(EditAnywhere, Category = "Target Filter View")
	uint8 bComplexTrace : 1;
	
private:
	/** Method to process the trace task immediately */
	void ExecuteImmediateTrace(const FTargetingRequestHandle& TargetingHandle) const;
	
	/** Setup CollisionQueryParams for the trace */
	void InitCollisionParams(const FTargetingRequestHandle& TargetingHandle, FCollisionQueryParams& OutParams) const;
};
