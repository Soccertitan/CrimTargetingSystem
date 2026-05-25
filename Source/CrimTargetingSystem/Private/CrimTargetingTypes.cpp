// Copyright Soccertitan 2026


#include "CrimTargetingTypes.h"

#include "TargetPointInterface.h"


FCrimTargetPoint::FCrimTargetPoint(const FHitResult& HitResult)
{
	WeakSceneComponent = HitResult.Component;
	SocketName = HitResult.BoneName;
	WeakActor = HitResult.GetActor();
}

FCrimTargetPoint::FCrimTargetPoint(AActor* Actor)
{
	WeakActor = Actor;
}

FCrimTargetPoint::FCrimTargetPoint(USceneComponent* SceneComponent, FName InSocketName)
{
	if (SceneComponent)
	{
		WeakSceneComponent = SceneComponent;
		SocketName = InSocketName;
		WeakActor = SceneComponent->GetAttachmentRootActor();
	}
}

bool FCrimTargetPoint::IsValid() const
{
	if (WeakSceneComponent.Get() || WeakActor.Get())
	{
		return true;
	}
	
	return false;
}

AActor* FCrimTargetPoint::GetActor() const
{
	return WeakActor.Get();
}

USceneComponent* FCrimTargetPoint::GetSceneComponent() const
{
	return WeakSceneComponent.Get();
}

bool FCrimTargetPoint::IsTargetable(AController* Controller) const
{
	if (Controller)
	{
		if (USceneComponent* StrongSceneComp = WeakSceneComponent.Get())
		{
			if (StrongSceneComp->Implements<UTargetPointInterface>())
			{
				return ITargetPointInterface::Execute_IsTargetPointTargetable(StrongSceneComp, Controller);
			}
			return true;
		}
	
		if (AActor* StrongActor = WeakActor.Get())
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
	if (const USceneComponent* StrongSceneComp = WeakSceneComponent.Get())
	{
		return StrongSceneComp->GetSocketLocation(SocketName);
	}
	
	if (const AActor* StrongActor = WeakActor.Get())
	{
		return StrongActor->GetActorLocation();
	}
	
	return FVector();
}

void FCrimTargetPoint::Reset()
{
	WeakSceneComponent = nullptr;
	SocketName = FName();
	WeakActor = nullptr;
}
