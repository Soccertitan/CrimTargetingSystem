// Copyright Soccertitan 2026


#include "TargetingTask/TargetingFilterTask_IsInView.h"

#include "CrimTargetingSystemBlueprintFunctionLibrary.h"
#include "TargetingTask/CrimTargetingViewInfo.h"

UTargetingFilterTask_IsInView::UTargetingFilterTask_IsInView(const FObjectInitializer& ObjectInitializer)
:Super(ObjectInitializer)
{
	TargetingViewInfo = CreateDefaultSubobject<UCrimTargetingViewInfo>(TEXT("TargetingViewInfo"));
}

void UTargetingFilterTask_IsInView::Execute(const FTargetingRequestHandle& TargetingHandle) const
{
	Super::Execute(TargetingHandle);
	
	SetTaskAsyncState(TargetingHandle, ETargetingTaskAsyncState::Executing);

	if (TargetingHandle.IsValid())
	{
		if (FTargetingDefaultResultsSet* ResultData = FTargetingDefaultResultsSet::Find(TargetingHandle))
		{
			FMinimalViewInfo ViewInfo;
			TargetingViewInfo->GetViewInfo(TargetingHandle, ViewInfo);
			
			const int32 NumTargets = ResultData->TargetResults.Num();
			for (int32 TargetIterator = NumTargets - 1; TargetIterator >= 0; --TargetIterator)
			{
				const FTargetingDefaultResultData& TargetResult = ResultData->TargetResults[TargetIterator];
				if (UCrimTargetingSystemBlueprintFunctionLibrary::IsLocationWithinView(ViewInfo, 
					TargetResult.HitResult.GetComponent()->GetSocketLocation(TargetResult.HitResult.BoneName)) == false)
				{
					ResultData->TargetResults.RemoveAtSwap(TargetIterator, EAllowShrinking::No);
				}
			}
		}
	}

	SetTaskAsyncState(TargetingHandle, ETargetingTaskAsyncState::Completed);
}
