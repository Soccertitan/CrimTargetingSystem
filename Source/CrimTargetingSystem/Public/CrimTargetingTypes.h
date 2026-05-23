// Copyright Soccertitan 2026

#pragma once

#include "CoreMinimal.h"
#include "CrimTargetingTypes.generated.h"


/** 
 * Used by the LockOnSystemComponent to lock onto points of interest. Prioritizes the SceneComponent if
 * both SceneComp and Actor is valid.
 */
USTRUCT(BlueprintType)
struct CRIMTARGETINGSYSTEM_API FCrimTargetPoint
{
	GENERATED_BODY()
	
	UPROPERTY(BlueprintReadWrite)
	TWeakObjectPtr<USceneComponent> SceneComponent;
	UPROPERTY(BlueprintReadWrite)
	FName SocketName;
	
	UPROPERTY(BlueprintReadWrite)
	TWeakObjectPtr<AActor> Actor;
	
	bool IsValid() const;
	AActor* GetActor() const;
	bool IsTargetable(APlayerController* PlayerController) const;
	FVector GetLocation() const;

	void Reset();
	
	friend bool operator==(const FCrimTargetPoint& X, const FCrimTargetPoint& Y)
	{
		return X.Actor == Y.Actor && X.SceneComponent == Y.SceneComponent && X.SocketName == Y.SocketName;
	}
	
	friend bool operator!=(const FCrimTargetPoint& X, const FCrimTargetPoint& Y)
	{
		return !(X == Y);
	}
};
