// Copyright Soccertitan 2026


#include "TargetingTask/CrimViewTargetingTask.h"

void UCrimViewTargetingTask::GetViewInfo(const FTargetingRequestHandle& TargetingHandle, FMinimalViewInfo& OutResult) const
{
	if (const FTargetingSourceContext* Context = FTargetingSourceContext::Find(TargetingHandle))
	{
		if (bUseActorEyes)
		{
			OutResult.FOV = FieldOfView.GetValue();
			if (bPrioritizeSourceActor)
			{
				if (Context->SourceActor)
				{
					Context->SourceActor->GetActorEyesViewPoint(OutResult.Location, OutResult.Rotation);
				}
				else if (Context->InstigatorActor)
				{
					Context->InstigatorActor->GetActorEyesViewPoint(OutResult.Location, OutResult.Rotation);
				}
			}
			else
			{
				if (Context->InstigatorActor)
				{
					Context->InstigatorActor->GetActorEyesViewPoint(OutResult.Location, OutResult.Rotation);
				}
				else if (Context->SourceActor)
				{
					Context->SourceActor->GetActorEyesViewPoint(OutResult.Location, OutResult.Rotation);
				}
			}
		}
		else
		{
			if (bPrioritizeSourceActor)
			{
				if (Context->SourceActor)
				{
					Context->SourceActor->CalcCamera(0.f, OutResult);
				}
				else if (Context->InstigatorActor)
				{
					Context->InstigatorActor->CalcCamera(0.f, OutResult);
				}
			}
			else
			{
				if (Context->InstigatorActor)
				{
					Context->InstigatorActor->CalcCamera(0.f, OutResult);
				}
				else if (Context->SourceActor)
				{
					Context->SourceActor->CalcCamera(0.f, OutResult);
				}
			}
		}
	}
}
