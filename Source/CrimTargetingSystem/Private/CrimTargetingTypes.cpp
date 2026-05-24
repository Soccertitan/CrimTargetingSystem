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

bool FCrimTargetPoint::IsTargetable(APlayerController* PlayerController) const
{
	if (PlayerController)
	{
		if (USceneComponent* StrongSceneComp = SceneComponent.Get())
		{
			if (StrongSceneComp->Implements<UTargetPointInterface>())
			{
				return ITargetPointInterface::Execute_IsTargetPointTargetable(StrongSceneComp, PlayerController);
			}
			return true;
		}
	
		if (AActor* StrongActor = Actor.Get())
		{
			if (StrongActor->Implements<UTargetPointInterface>())
			{
				return ITargetPointInterface::Execute_IsTargetPointTargetable(StrongActor, PlayerController);
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
