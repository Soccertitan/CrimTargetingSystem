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
	
	FCrimTargetPoint(){}
	FCrimTargetPoint(const FHitResult& HitResult);
	FCrimTargetPoint(AActor* Actor);
	FCrimTargetPoint(USceneComponent* SceneComponent, FName InSocketName);
	
	bool IsValid() const;
	AActor* GetActor() const;
	USceneComponent* GetSceneComponent() const;
	FName GetSocketName() const { return SocketName; }
	bool IsTargetable(AController* Controller) const;
	FVector GetLocation() const;

	void Reset();
	
	friend bool operator==(const FCrimTargetPoint& X, const FCrimTargetPoint& Y)
	{
		return X.WeakActor == Y.WeakActor && X.WeakSceneComponent == Y.WeakSceneComponent && X.SocketName == Y.SocketName;
	}
	
	friend bool operator!=(const FCrimTargetPoint& X, const FCrimTargetPoint& Y)
	{
		return !(X == Y);
	}
	
private:
	UPROPERTY(BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	TWeakObjectPtr<USceneComponent> WeakSceneComponent;
	UPROPERTY(BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	FName SocketName;
	
	UPROPERTY(BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	TWeakObjectPtr<AActor> WeakActor;
};
