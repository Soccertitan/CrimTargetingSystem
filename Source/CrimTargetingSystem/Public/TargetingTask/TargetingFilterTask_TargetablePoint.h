// Copyright Soccertitan 2026

#pragma once

#include "CoreMinimal.h"
#include "Tasks/TargetingFilterTask_BasicFilterTemplate.h"

#include "TargetingFilterTask_TargetablePoint.generated.h"

/**
 * Filters out results that are not targetable using the TargetPointInterface.
 */
UCLASS()
class CRIMTARGETINGSYSTEM_API UTargetingFilterTask_TargetablePoint : public UTargetingFilterTask_BasicFilterTemplate
{
	GENERATED_BODY()
	
protected:
	/** If true, the filter will not remove the HitResult if it does not implement the TargetPointInterface. */
	UPROPERTY(EditAnywhere, Category = "Target Filter TargetPoint")
	bool bIgnoreTargetPointInterface = true;
	
	virtual bool ShouldFilterTarget(const FTargetingRequestHandle& TargetingHandle, const FTargetingDefaultResultData& TargetData) const override;
};
