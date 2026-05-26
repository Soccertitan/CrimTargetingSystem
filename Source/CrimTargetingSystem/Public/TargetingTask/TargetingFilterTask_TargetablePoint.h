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
	
public:
	/** If false, the filter will not remove the HitResult if it does not implement the TargetPointInterface. */
	UPROPERTY(EditAnywhere, Category = "Data")
	bool bFilterResultIfDoesNotImplementInterface = false;
	
protected:
	virtual bool ShouldFilterTarget(const FTargetingRequestHandle& TargetingHandle, const FTargetingDefaultResultData& TargetData) const override;
};
