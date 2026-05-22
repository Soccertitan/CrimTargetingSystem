// Copyright Soccertitan 2026

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "LockOnSystemBlueprintFunctionLibrary.generated.h"

struct FCrimTargetPoint;
/**
 * 
 */
UCLASS()
class CRIMLOCKONSYSTEM_API ULockOnSystemBlueprintFunctionLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()
	
public:
	UFUNCTION(BlueprintPure, Category = "LockOn System", DisplayName = "IsValid")
	static bool IsLockOnPointValid(const FCrimTargetPoint& LockOnPoint);
	
	/** Will return true if the PlayerController can lock on to the target. */
	UFUNCTION(BlueprintPure, Category = "LockOn System")
	static bool IsLockOnPointTargetable(APlayerController* PlayerController, const FCrimTargetPoint& LockOnPoint);
	
	UFUNCTION(BlueprintPure, Category = "LockOn System")
	static FVector GetLockOnPointLocation(const FCrimTargetPoint& LockOnPoint);
	
	/** Returns the actor that is in the LockOnPoint. */
	UFUNCTION(BlueprintPure, Category = "LockOn System")
	static AActor* GetActor(const FCrimTargetPoint& LockOnPoint);
};
