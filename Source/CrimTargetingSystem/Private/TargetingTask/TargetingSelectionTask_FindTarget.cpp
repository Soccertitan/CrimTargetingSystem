// Copyright Soccertitan 2026


#include "TargetingTask/TargetingSelectionTask_FindTarget.h"

#include "CrimTargetingSystemBlueprintFunctionLibrary.h"
#include "Kismet/GameplayStatics.h"

UTargetingSelectionTask_FindTarget::UTargetingSelectionTask_FindTarget(const FObjectInitializer& ObjectInitializer)
:Super(ObjectInitializer)
{
	bComplexTrace = false;
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
		FMinimalViewInfo ViewInfo;
		GetViewInfo(TargetingHandle, ViewInfo);
		FMatrix ViewMatrix;
		FMatrix ProjectionMatrix;
		FMatrix ViewProjectionMatrix;
		UGameplayStatics::GetViewProjectionMatrix(ViewInfo, ViewMatrix, ProjectionMatrix, ViewProjectionMatrix);
		
		float ClosestDistance = TNumericLimits<float>::Max();
		int32 ClosestIndex = -1;
		for (int32 Index = 0; Index < ResultData->TargetResults.Num(); Index++)
		{
			const FHitResult& HitResult = ResultData->TargetResults[Index].HitResult;
			FVector2D ScreenPos;
			
			const bool bIsValidTarget = IsValidTarget(TargetingHandle, ViewInfo, ViewProjectionMatrix, HitResult, ScreenPos);
			if (bIsValidTarget == false)
			{
				continue;
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

void UTargetingSelectionTask_FindTarget::FindTarget(const FTargetingRequestHandle& TargetingHandle) const
{
	if (FTargetingDefaultResultsSet* ResultData = FTargetingDefaultResultsSet::Find(TargetingHandle))
	{
		const FVector2D SearchDirection = GetSearchDirection(TargetingHandle).GetSafeNormal();
		const float SearchAngle = GetSearchAngle(TargetingHandle);
		const FVector OriginLocation = GetOriginLocation(TargetingHandle);
		const bool bWrapSearch = GetWrapSearch(TargetingHandle);
		
		FMinimalViewInfo ViewInfo;
		GetViewInfo(TargetingHandle, ViewInfo);
		FMatrix ViewMatrix;
		FMatrix ProjectionMatrix;
		FMatrix ViewProjectionMatrix;
		UGameplayStatics::GetViewProjectionMatrix(ViewInfo, ViewMatrix, ProjectionMatrix, ViewProjectionMatrix);
		
		FVector2D OriginScreenPos;
		UCrimTargetingSystemBlueprintFunctionLibrary::ProjectWorldPointToViewProjectionMatrix(ViewProjectionMatrix, OriginLocation, OriginScreenPos);
		
		// The target to select when searching in the direction.
		int32 NextTargetIndex = -1;
		float NextComparison = TNumericLimits<float>::Max();
		// The target to select when wrapping around the opposite direction of origin. 
		int32 PreviousTargetIndex = -1;
		float PreviousComparison = 0.f;
		
		for (int32 Index = 0; Index < ResultData->TargetResults.Num(); Index++)
		{
			const FHitResult& HitResult = ResultData->TargetResults[Index].HitResult;
			FVector2D TargetScreenPos;
			const bool bIsValidTarget = IsValidTarget(TargetingHandle, ViewInfo, ViewProjectionMatrix, HitResult, TargetScreenPos);
			if (bIsValidTarget == false)
			{
				continue;
			}
			
			// Skip a target that has the same screen positioning as the origin.
			if (OriginScreenPos == TargetScreenPos)
			{
				continue;
			}
			
			const float Score = CalculateTargetScore(TargetingHandle, OriginScreenPos, TargetScreenPos, SearchDirection, SearchAngle);
			if (Score > 0.f && Score <= NextComparison)
			{
				NextComparison = Score;
				NextTargetIndex = Index;
			}
			else if (bWrapSearch && Score < 0.f && Score <= PreviousComparison)
			{
				PreviousComparison = Score;
				PreviousTargetIndex = Index;
			}
		}
		
		if (NextTargetIndex >= 0 || PreviousTargetIndex >= 0)
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
			ResultData->TargetResults.Empty();
		}
	}
}

FVector2D UTargetingSelectionTask_FindTarget::GetSearchDirection(const FTargetingRequestHandle& TargetingHandle) const
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

void UTargetingSelectionTask_FindTarget::GetViewInfo(const FTargetingRequestHandle& TargetingHandle, FMinimalViewInfo& OutResult) const
{
	OutResult = DefaultViewInfo;
	if (const FTargetingSourceContext* Context = FTargetingSourceContext::Find(TargetingHandle))
	{
		if (Context->SourceActor)
		{
			Context->SourceActor->CalcCamera(0.f, OutResult);
		}
	}
}

float UTargetingSelectionTask_FindTarget::GetSearchAngle(const FTargetingRequestHandle& TargetingHandle) const
{
	return DefaultSearchAngle;
}

bool UTargetingSelectionTask_FindTarget::GetWrapSearch(const FTargetingRequestHandle& TargetingHandle) const
{
	return bDefaultWrapSearch;
}

bool UTargetingSelectionTask_FindTarget::IsValidTarget(const FTargetingRequestHandle& TargetingHandle, const FMinimalViewInfo& ViewInfo, 
                                                       const FMatrix& ViewProjectionMatrix, const FHitResult& HitResult, FVector2D& OutScreenPos) const
{
	const bool bInViewProjection = UCrimTargetingSystemBlueprintFunctionLibrary::ProjectWorldPointToViewProjectionMatrix(
		ViewProjectionMatrix, HitResult.GetComponent()->GetSocketLocation(HitResult.BoneName), OutScreenPos);
	
	if (bInViewProjection == false)
	{
		return false;
	}
	
	const bool bViewBlocked = IsViewBlocked(TargetingHandle, ViewInfo.Location, HitResult.GetComponent()->GetSocketLocation(HitResult.BoneName));
	return !bViewBlocked;
}

float UTargetingSelectionTask_FindTarget::CalculateTargetScore(const FTargetingRequestHandle& TargetingHandle, const FVector2D& Origin, const FVector2D& Target, const FVector2D& Direction, const float SearchAngle) const
{
	const FVector2D Delta = Target - Origin;
	
	const float DotProduct = Delta.GetSafeNormal().Dot(Direction);
	const float Degrees = FMath::RadiansToDegrees(FGenericPlatformMath::Acos(FMath::Abs(DotProduct)));
	const int32 Sign = FMath::Sign(DotProduct);
	if (Degrees > SearchAngle)
	{
		return 0.f;
	}
	
	// Multiply by sign so that distance is appropriate for searching in the direction.
	float Distance = FVector2D::Distance(Target * Direction, Origin * Direction) * Sign;
	
	return Distance;
}

bool UTargetingSelectionTask_FindTarget::IsViewBlocked(const FTargetingRequestHandle& TargetingHandle, const FVector& Start, const FVector& End) const
{
	if (UWorld* World = GetSourceContextWorld(TargetingHandle))
	{
		FHitResult LineTraceHitResult;
		FCollisionQueryParams Params(FName("LineTraceSingle"), bComplexTrace);
		InitCollisionParams(TargetingHandle, Params);
		const ECollisionChannel CollisionChannel = UEngineTypes::ConvertToCollisionChannel(TraceChannel);
				
		bool bBlockedHit = World->LineTraceSingleByChannel(LineTraceHitResult, 
			Start, 
			End,
			CollisionChannel,
			Params);
		
		return bBlockedHit;
	}
	return true;
}

void UTargetingSelectionTask_FindTarget::InitCollisionParams(const FTargetingRequestHandle& TargetingHandle, FCollisionQueryParams& OutParams) const
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
