// Copyright Soccertitan 2026


#include "LockOnTypes.h"

#include "LockOnPointInterface.h"


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
	if (USceneComponent* StrongSceneComp = SceneComponent.Get())
	{
		if (StrongSceneComp->Implements<ULockOnPointInterface>())
		{
			return ILockOnPointInterface::Execute_IsLockOnPointTargetable(StrongSceneComp, PlayerController);
		}
		return true;
	}
	
	if (AActor* StrongActor = Actor.Get())
	{
		if (StrongActor->Implements<ULockOnPointInterface>())
		{
			return ILockOnPointInterface::Execute_IsLockOnPointTargetable(StrongActor, PlayerController);
		}
		return true;
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
