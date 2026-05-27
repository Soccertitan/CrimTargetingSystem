// Copyright Soccertitan 2026


#include "TargetingTask/TargetingFilterTask_IsInView.h"

#include "CrimTargetingSystemBlueprintFunctionLibrary.h"
#include "Kismet/KismetMathLibrary.h"

void UTargetingFilterTask_IsInView::Execute(const FTargetingRequestHandle& TargetingHandle) const
{
	Super::Execute(TargetingHandle);
	
	SetTaskAsyncState(TargetingHandle, ETargetingTaskAsyncState::Executing);

	if (TargetingHandle.IsValid())
	{
		if (FTargetingDefaultResultsSet* ResultData = FTargetingDefaultResultsSet::Find(TargetingHandle))
		{
			FMinimalViewInfo ViewInfo;
			GetViewInfo(TargetingHandle, ViewInfo);
			
			const int32 NumTargets = ResultData->TargetResults.Num();
			for (int32 TargetIterator = NumTargets - 1; TargetIterator >= 0; --TargetIterator)
			{
				const FTargetingDefaultResultData& TargetResult = ResultData->TargetResults[TargetIterator];
				if (UCrimTargetingSystemBlueprintFunctionLibrary::IsVectorInView(ViewInfo, 
					TargetResult.HitResult.GetComponent()->GetSocketLocation(TargetResult.HitResult.BoneName)) == false)
				{
					ResultData->TargetResults.RemoveAtSwap(TargetIterator, EAllowShrinking::No);
				}
			}
		}
	}

	SetTaskAsyncState(TargetingHandle, ETargetingTaskAsyncState::Completed);
}
