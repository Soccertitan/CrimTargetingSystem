// Copyright Soccertitan 2026


#include "TargetingTask/TargetingFilterTask_IsViewBlocked.h"

#include "TargetingTask/CrimTargetingViewInfo.h"

UTargetingFilterTask_IsViewBlocked::UTargetingFilterTask_IsViewBlocked(const FObjectInitializer& ObjectInitializer)
:Super(ObjectInitializer)
{
	bComplexTrace = false;
	TargetingViewInfo = CreateDefaultSubobject<UCrimTargetingViewInfo>(TEXT("TargetingViewInfo"));
}

void UTargetingFilterTask_IsViewBlocked::Execute(const FTargetingRequestHandle& TargetingHandle) const
{
	Super::Execute(TargetingHandle);
	
	SetTaskAsyncState(TargetingHandle, ETargetingTaskAsyncState::Executing);
	
	ExecuteImmediateTrace(TargetingHandle);
}

void UTargetingFilterTask_IsViewBlocked::ExecuteImmediateTrace(const FTargetingRequestHandle& TargetingHandle) const
{
	if (UWorld* World = GetSourceContextWorld(TargetingHandle))
	{
		if (FTargetingDefaultResultsSet* ResultData = FTargetingDefaultResultsSet::Find(TargetingHandle))
		{
			FMinimalViewInfo ViewInfo;
			TargetingViewInfo->GetViewInfo(TargetingHandle, ViewInfo);
			const FVector Start = ViewInfo.Location;
			
			const int32 NumTargets = ResultData->TargetResults.Num();
			for (int32 TargetIterator = NumTargets - 1; TargetIterator >= 0; --TargetIterator)
			{
				const FTargetingDefaultResultData& TargetResult = ResultData->TargetResults[TargetIterator];
				FHitResult LineTraceHitResult;
				FCollisionQueryParams Params(FName("LineTraceSingle"), bComplexTrace);
				InitCollisionParams(TargetingHandle, Params);
				const ECollisionChannel CollisionChannel = UEngineTypes::ConvertToCollisionChannel(TraceChannel);
				
				bool bBlockedHit = World->LineTraceSingleByChannel(LineTraceHitResult, 
					Start, 
					TargetResult.HitResult.GetComponent()->GetSocketLocation(TargetResult.HitResult.BoneName),
					CollisionChannel,
					Params);
				
				if (bBlockedHit)
				{
					ResultData->TargetResults.RemoveAtSwap(TargetIterator, EAllowShrinking::No);
				}
			}
		}
	}
	
	SetTaskAsyncState(TargetingHandle, ETargetingTaskAsyncState::Completed);
}

void UTargetingFilterTask_IsViewBlocked::InitCollisionParams(const FTargetingRequestHandle& TargetingHandle, FCollisionQueryParams& OutParams) const
{
	if (const FTargetingSourceContext* SourceContext = FTargetingSourceContext::Find(TargetingHandle))
	{
		if (SourceContext->SourceActor)
		{
			OutParams.AddIgnoredActor(SourceContext->SourceActor);
		}

		if (SourceContext->InstigatorActor)
		{
			OutParams.AddIgnoredActor(SourceContext->InstigatorActor);
		}
	}
}
