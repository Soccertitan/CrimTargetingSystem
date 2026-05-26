// Copyright Soccertitan 2026


#include "TargetingTask/TargetingSelectionTask_FindNextTarget.h"

#include "CrimTargetingSystemBlueprintFunctionLibrary.h"
#include "Kismet/KismetMathLibrary.h"

void UTargetingSelectionTask_FindNextTarget::Execute(const FTargetingRequestHandle& TargetingHandle) const
{
	Super::Execute(TargetingHandle);
	
	SetTaskAsyncState(TargetingHandle, ETargetingTaskAsyncState::Executing);
	
	if (GetOriginLocation(TargetingHandle) == FVector::ZeroVector)
	{
		FindNearestTarget(TargetingHandle);
	}
	else
	{
		FindNextTarget(TargetingHandle);
	}
	
	SetTaskAsyncState(TargetingHandle, ETargetingTaskAsyncState::Completed);
}

void UTargetingSelectionTask_FindNextTarget::FindNearestTarget(const FTargetingRequestHandle& TargetingHandle) const
{
	if (FTargetingDefaultResultsSet* ResultData = FTargetingDefaultResultsSet::Find(TargetingHandle))
	{
		FMinimalViewInfo ViewInfo;
		GetViewInfo(TargetingHandle, ViewInfo);
		FVector ForwardViewVector = UKismetMathLibrary::GetForwardVector(ViewInfo.Rotation);
		ForwardViewVector.Normalize();
		
		float ClosestDistance = TNumericLimits<float>::Max();
		int32 ClosestIndex = -1;
		for (int32 Index = 0; Index < ResultData->TargetResults.Num(); Index++)
		{
			const FHitResult& HitResult = ResultData->TargetResults[Index].HitResult;
			
			if (bTargetMustBeInView == false)
			{
				FVector TargetVector = HitResult.Location - ViewInfo.Location;
				TargetVector.Normalize();
				
				const float ForwardDotProduct = FVector::DotProduct(TargetVector, ForwardViewVector);
				// Skip targeting a hit if it's behind the view.
				if (ForwardDotProduct < 0.f)
				{
					continue;
				}
			}
			
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

void UTargetingSelectionTask_FindNextTarget::FindNextTarget(const FTargetingRequestHandle& TargetingHandle) const
{
	if (FTargetingDefaultResultsSet* ResultData = FTargetingDefaultResultsSet::Find(TargetingHandle))
	{
		const EUINavigation SearchDirection = GetSearchDirection(TargetingHandle);
		
		const FVector OriginLocation = GetOriginLocation(TargetingHandle);
		
		FMinimalViewInfo ViewInfo;
		GetViewInfo(TargetingHandle, ViewInfo);
		
		FVector RightViewVector = UKismetMathLibrary::GetRightVector(ViewInfo.Rotation);
		RightViewVector.Normalize();
		FVector UpViewVector = UKismetMathLibrary::GetUpVector(ViewInfo.Rotation);
		UpViewVector.Normalize();
		
		// The target to select if searching Right/Up
		int32 NextTargetIndex = -1;
		float NextComparison = 0.f;
		// The target to select if searching Left/Down
		int32 PreviousTargetIndex = -1;
		float PreviousComparison = 0.f;

		if (SearchDirection == EUINavigation::Right ||
			SearchDirection == EUINavigation::Up)
		{
			NextComparison = 1.f;
		}
		else
		{
			PreviousComparison = -1.f;
		}
		
		for (int32 Index = 0; Index < ResultData->TargetResults.Num(); Index++)
		{
			const FHitResult& HitResult = ResultData->TargetResults[Index].HitResult;
			
			if (bTargetMustBeInView == false)
			{
				const FVector TargetVector = HitResult.GetComponent()->GetSocketLocation(HitResult.BoneName);
				// Skip targeting a hit if it's behind the view.
				if (!UCrimTargetingSystemBlueprintFunctionLibrary::IsVectorInView(ViewInfo, TargetVector))
				{
					continue;
				}
			}
			
			FVector DeltaVector = HitResult.GetComponent()->GetSocketLocation(HitResult.BoneName) - OriginLocation;
			DeltaVector.Normalize();
			
			if (SearchDirection == EUINavigation::Right || 
				SearchDirection == EUINavigation::Left)
			{
				const float RightDotProduct = FVector::DotProduct(DeltaVector, RightViewVector);
				if (SearchDirection == EUINavigation::Right)
				{
					if (RightDotProduct > RightLeftTargetThreshold && RightDotProduct <= NextComparison)
					{
						NextComparison = RightDotProduct;
						NextTargetIndex = Index;
					}
					else if (RightDotProduct < -RightLeftTargetThreshold && RightDotProduct <= PreviousComparison)
					{
						PreviousComparison = RightDotProduct;
						PreviousTargetIndex = Index;
					}
				}
				else
				{
					if (RightDotProduct > RightLeftTargetThreshold && RightDotProduct >= NextComparison)
					{
						NextComparison = RightDotProduct;
						NextTargetIndex = Index;
					}
					else if (RightDotProduct < -RightLeftTargetThreshold && RightDotProduct >= PreviousComparison)
					{
						PreviousComparison = RightDotProduct;
						PreviousTargetIndex = Index;
					}
				}
			}
			else
			{
				// Searching Up and down
				const float UpDotProduct = FVector::DotProduct(DeltaVector, UpViewVector);
				if (SearchDirection == EUINavigation::Up)
				{
					if (UpDotProduct > UpDownTargetThreshold && UpDotProduct <= NextComparison)
					{
						NextComparison = UpDotProduct;
						NextTargetIndex = Index;
					}
					else if (UpDotProduct < -UpDownTargetThreshold && UpDotProduct <= PreviousComparison)
					{
						PreviousComparison = UpDotProduct;
						PreviousTargetIndex = Index;
					}
				}
				else
				{
					if (UpDotProduct > UpDownTargetThreshold && UpDotProduct >= NextComparison)
					{
						NextComparison = UpDotProduct;
						NextTargetIndex = Index;
					}
					else if (UpDotProduct < -UpDownTargetThreshold && UpDotProduct >= PreviousComparison)
					{
						PreviousComparison = UpDotProduct;
						PreviousTargetIndex = Index;
					}
				}
			}
		}
		
		if (NextTargetIndex >= 0 || PreviousTargetIndex >= 0)
		{
			if (SearchDirection == EUINavigation::Right ||
				SearchDirection == EUINavigation::Up)
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

EUINavigation UTargetingSelectionTask_FindNextTarget::GetSearchDirection(const FTargetingRequestHandle& TargetingHandle) const
{
	return DefaultSearchDirection;
}

FVector UTargetingSelectionTask_FindNextTarget::GetOriginLocation(const FTargetingRequestHandle& TargetingHandle) const
{
	if (const FTargetingSourceContext* Context = FTargetingSourceContext::Find(TargetingHandle))
	{
		return Context->SourceLocation;
	}
	return FVector::ZeroVector;
}

void UTargetingSelectionTask_FindNextTarget::GetViewInfo(const FTargetingRequestHandle& TargetingHandle, FMinimalViewInfo& OutResult) const
{
	if (const FTargetingSourceContext* Context = FTargetingSourceContext::Find(TargetingHandle))
	{
		if (Context->SourceActor)
		{
			Context->SourceActor->CalcCamera(0.f, OutResult);
		}
	}
}
