// Copyright Soccertitan 2026


#include "TargetingTask/TargetingFilterTask_TargetablePoint.h"

#include "TargetPointInterface.h"

bool UTargetingFilterTask_TargetablePoint::ShouldFilterTarget(const FTargetingRequestHandle& TargetingHandle, const FTargetingDefaultResultData& TargetData) const
{
	if (const FTargetingSourceContext* Context = FTargetingSourceContext::Find(TargetingHandle))
	{
		if (UPrimitiveComponent* StrongComp = TargetData.HitResult.GetComponent())
		{
			if (StrongComp->Implements<UTargetPointInterface>())
			{
				return !ITargetPointInterface::Execute_IsTargetPointTargetable(StrongComp, Context->SourceActor);
			}
		}
		
		if (AActor* StrongActor = TargetData.HitResult.GetActor())
		{
			if (StrongActor->Implements<UTargetPointInterface>())
			{
				return !ITargetPointInterface::Execute_IsTargetPointTargetable(StrongActor, Context->SourceActor);
			}
		}
	}
	return bFilterResultIfDoesNotImplementInterface;
}
