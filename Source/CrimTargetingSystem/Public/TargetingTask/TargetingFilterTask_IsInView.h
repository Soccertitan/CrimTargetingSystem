// Copyright Soccertitan 2026

#pragma once

#include "CoreMinimal.h"
#include "CrimViewTargetingTask.h"
#include "ScalableFloat.h"
#include "TargetingFilterTask_IsInView.generated.h"

/**
 * Filters out targets that are not in front of the view.
 */
UCLASS()
class CRIMTARGETINGSYSTEM_API UTargetingFilterTask_IsInView : public UCrimViewTargetingTask
{
	GENERATED_BODY()
	
public:
	virtual void Execute(const FTargetingRequestHandle& TargetingHandle) const override;
};
