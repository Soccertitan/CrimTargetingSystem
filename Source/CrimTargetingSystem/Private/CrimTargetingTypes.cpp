// Copyright Soccertitan 2026


#include "CrimTargetingTypes.h"

#include "TargetPointInterface.h"


bool FCrimTargetPoint::IsValid() const
{
	if (SceneComponent.Get() || Actor.Get())
	{
		return true;
	}
	
	return false;
}

AActor* FCrimTargetPoint::GetActor() const
{
	if (const USceneComponent* StrongSceneComp = SceneComponent.Get())
	{
		return StrongSceneComp->GetOwner();
	}

	return Actor.Get();
}

bool FCrimTargetPoint::IsTargetable(AController* Controller) const
{
	if (Controller)
	{
		if (USceneComponent* StrongSceneComp = SceneComponent.Get())
		{
			if (StrongSceneComp->Implements<UTargetPointInterface>())
			{
				return ITargetPointInterface::Execute_IsTargetPointTargetable(StrongSceneComp, Controller);
			}
			return true;
		}
	
		if (AActor* StrongActor = Actor.Get())
		{
			if (StrongActor->Implements<UTargetPointInterface>())
			{
				return ITargetPointInterface::Execute_IsTargetPointTargetable(StrongActor, Controller);
			}
			return true;
		}
	}
	
	return false;
}

FVector FCrimTargetPoint::GetLocation() const
{
	if (const USceneComponent* StrongSceneComp = SceneComponent.Get())
	{
		return StrongSceneComp->GetSocketLocation(SocketName);
	}
	
	if (const AActor* StrongActor = Actor.Get())
	{
		return StrongActor->GetActorLocation();
	}
	
	return FVector();
}

void FCrimTargetPoint::Reset()
{
	SceneComponent = nullptr;
	SocketName = FName();
	Actor = nullptr;
}
