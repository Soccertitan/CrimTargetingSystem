// Copyright Soccertitan 2026


#include "TargetingTask/TargetingSelectionTask_FindTarget.h"

#include "Kismet/KismetMathLibrary.h"
#include "TargetingTask/CrimTargetingViewInfo.h"

UTargetingSelectionTask_FindTarget::UTargetingSelectionTask_FindTarget(const FObjectInitializer& ObjectInitializer)
:Super(ObjectInitializer)
{
	TargetingViewInfo = CreateDefaultSubobject<UCrimTargetingViewInfo>(TEXT("TargetingViewInfo"));
}

void UTargetingSelectionTask_FindTarget::Execute(const FTargetingRequestHandle& TargetingHandle) const
{
	Super::Execute(TargetingHandle);
	
	SetTaskAsyncState(TargetingHandle, ETargetingTaskAsyncState::Executing);
	
	if (GetOriginLocation(TargetingHandle) == FVector::ZeroVector)
	{
		FindNearestTarget(TargetingHandle);
	}
	else
	{
		FindTarget(TargetingHandle);
	}
	
	SetTaskAsyncState(TargetingHandle, ETargetingTaskAsyncState::Completed);
}

void UTargetingSelectionTask_FindTarget::FindNearestTarget(const FTargetingRequestHandle& TargetingHandle) const
{
	if (FTargetingDefaultResultsSet* ResultData = FTargetingDefaultResultsSet::Find(TargetingHandle))
	{
		float ClosestDistance = TNumericLimits<float>::Max();
		int32 ClosestIndex = -1;
		for (int32 Index = 0; Index < ResultData->TargetResults.Num(); Index++)
		{
			const FHitResult& HitResult = ResultData->TargetResults[Index].HitResult;
			
			if (HitResult.Distance < ClosestDistance)
			{
				ClosestDistance = HitResult.Distance;
				ClosestIndex = Index;
			}
		}
		
		if (ResultData->TargetResults.IsValidIndex(ClosestIndex))
		{
			ResultData->TargetResults = {ResultData->TargetResults[ClosestIndex]};
		}
		else
		{
			ResultData->TargetResults.Empty();
		}
	}
}

void UTargetingSelectionTask_FindTarget::FindTarget(const FTargetingRequestHandle& TargetingHandle) const
{
	if (FTargetingDefaultResultsSet* ResultData = FTargetingDefaultResultsSet::Find(TargetingHandle))
	{
		const ETargetingSearchDirection SearchDirection = GetSearchDirection(TargetingHandle);
		const FVector OriginLocation = GetOriginLocation(TargetingHandle);
		const float DotProductTargetThreshold = GetDotProductTargetThreshold(TargetingHandle);
		
		FMinimalViewInfo ViewInfo;
		TargetingViewInfo->GetViewInfo(TargetingHandle, ViewInfo);
		
		FVector ViewDirection = GetSearchDirectionVector(SearchDirection, ViewInfo);
		ViewDirection.Normalize();
		
		// The target to select if searching Right/Up/Forward
		int32 NextTargetIndex = -1;
		float NextComparison = 0.f;
		// The target to select if searching Left/Down/Backward
		int32 PreviousTargetIndex = -1;
		float PreviousComparison = 0.f;

		if (SearchDirection == ETargetingSearchDirection::Right ||
			SearchDirection == ETargetingSearchDirection::Up ||
			SearchDirection == ETargetingSearchDirection::Forward)
		{
			// Searching on a positive axis, set the NextComparison to the biggest value.
			NextComparison = 1.f;
		}
		else
		{
			// Searching on the negative axis. set the starting comparision value to the lowest.
			PreviousComparison = -1.f;
		}
		
		for (int32 Index = 0; Index < ResultData->TargetResults.Num(); Index++)
		{
			const FHitResult& HitResult = ResultData->TargetResults[Index].HitResult;
			
			FVector DeltaVector = HitResult.GetComponent()->GetSocketLocation(HitResult.BoneName) - OriginLocation;
			DeltaVector.Normalize();
			
			const float DirectionDotProduct = FVector::DotProduct(DeltaVector, ViewDirection);
			if (SearchDirection == ETargetingSearchDirection::Right || 
				SearchDirection == ETargetingSearchDirection::Up ||
				SearchDirection == ETargetingSearchDirection::Forward)
			{
				if (DirectionDotProduct > DotProductTargetThreshold && DirectionDotProduct <= NextComparison)
				{
					NextComparison = DirectionDotProduct;
					NextTargetIndex = Index;
				}
				else if (DirectionDotProduct < -DotProductTargetThreshold && DirectionDotProduct <= PreviousComparison)
				{
					PreviousComparison = DirectionDotProduct;
					PreviousTargetIndex = Index;
				}
			}
			else
			{
				// Searching in reverse for a target.
				if (DirectionDotProduct > DotProductTargetThreshold && DirectionDotProduct >= NextComparison)
				{
					NextComparison = DirectionDotProduct;
					NextTargetIndex = Index;
				}
				else if (DirectionDotProduct < -DotProductTargetThreshold && DirectionDotProduct >= PreviousComparison)
				{
					PreviousComparison = DirectionDotProduct;
					PreviousTargetIndex = Index;
				}
			}
		}
		
		if (NextTargetIndex >= 0 || PreviousTargetIndex >= 0)
		{
			if (SearchDirection == ETargetingSearchDirection::Right ||
				SearchDirection == ETargetingSearchDirection::Up ||
				SearchDirection == ETargetingSearchDirection::Forward)
			{
				if (ResultData->TargetResults.IsValidIndex(NextTargetIndex))
				{
					ResultData->TargetResults = {ResultData->TargetResults[NextTargetIndex]};
				}
				else if (ResultData->TargetResults.IsValidIndex(PreviousTargetIndex))
				{
					ResultData->TargetResults = {ResultData->TargetResults[PreviousTargetIndex]};
				}
			}
			else
			{
				if (ResultData->TargetResults.IsValidIndex(PreviousTargetIndex))
				{
					ResultData->TargetResults = {ResultData->TargetResults[PreviousTargetIndex]};
				}
				else if (ResultData->TargetResults.IsValidIndex(NextTargetIndex))
				{
					ResultData->TargetResults = {ResultData->TargetResults[NextTargetIndex]};
				}
			}
		}
		else
		{
			ResultData->TargetResults.Empty();
		}
	}
}

ETargetingSearchDirection UTargetingSelectionTask_FindTarget::GetSearchDirection(const FTargetingRequestHandle& TargetingHandle) const
{
	return DefaultSearchDirection;
}

FVector UTargetingSelectionTask_FindTarget::GetOriginLocation(const FTargetingRequestHandle& TargetingHandle) const
{
	if (const FTargetingSourceContext* Context = FTargetingSourceContext::Find(TargetingHandle))
	{
		return Context->SourceLocation;
	}
	return FVector::ZeroVector;
}

float UTargetingSelectionTask_FindTarget::GetDotProductTargetThreshold(const FTargetingRequestHandle& TargetingHandle) const
{
	return FMath::Clamp(DefaultDotProductTargetThreshold.GetValue(), 0.f, 1.f);
}

FVector UTargetingSelectionTask_FindTarget::GetSearchDirectionVector(ETargetingSearchDirection SearchDirection, const FMinimalViewInfo& ViewInfo)
{
	if (SearchDirection == ETargetingSearchDirection::Right ||
		SearchDirection == ETargetingSearchDirection::Left)
	{
		return UKismetMathLibrary::GetRightVector(ViewInfo.Rotation);
	}
	
	if (SearchDirection == ETargetingSearchDirection::Up ||
		SearchDirection == ETargetingSearchDirection::Down)
	{
		return UKismetMathLibrary::GetUpVector(ViewInfo.Rotation);
	}
	
	return UKismetMathLibrary::GetForwardVector(ViewInfo.Rotation);
}
