// Copyright Soccertitan 2026


#include "LockOnSystemBlueprintFunctionLibrary.h"

#include "LockOnTypes.h"


bool ULockOnSystemBlueprintFunctionLibrary::IsLockOnPointValid(const FCrimTargetPoint& LockOnPoint)
{
	return LockOnPoint.IsValid();
}

bool ULockOnSystemBlueprintFunctionLibrary::IsLockOnPointTargetable(APlayerController* PlayerController, const FCrimTargetPoint& LockOnPoint)
{
	return LockOnPoint.IsTargetable(PlayerController);
}

FVector ULockOnSystemBlueprintFunctionLibrary::GetLockOnPointLocation(const FCrimTargetPoint& LockOnPoint)
{
	return LockOnPoint.GetLocation();
}

AActor* ULockOnSystemBlueprintFunctionLibrary::GetActor(const FCrimTargetPoint& LockOnPoint)
{
	return LockOnPoint.GetActor();
}
