// Copyright Soccertitan 2026

#pragma once

#include "CoreMinimal.h"
#include "Tasks/TargetingTask.h"
#include "TargetingFilterTask_IsInView.generated.h"

class UCrimTargetingViewInfo;

/**
 * Filters out targets that are not within the view.
 */
UCLASS()
class CRIMTARGETINGSYSTEM_API UTargetingFilterTask_IsInView : public UTargetingTask
{
	GENERATED_BODY()
	
public:
	UTargetingFilterTask_IsInView(const FObjectInitializer& ObjectInitializer);
	
	virtual void Execute(const FTargetingRequestHandle& TargetingHandle) const override;
	
protected:
	UPROPERTY(EditAnywhere, Category = "Target View", NoClear)
	TObjectPtr<UCrimTargetingViewInfo> TargetingViewInfo;
};
