// Copyright Soccertitan 2026

#pragma once

#include "CoreMinimal.h"
#include "LockOnTypes.h"
#include "Components/ActorComponent.h"
#include "LockOnSystemComponent.generated.h"


class ULockOnPointComponent;
class UCharacterMovementComponent;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FLockOnSystemLockOnPointSignature, const FLockOnPoint&, OldLockOnPoint, const FCrimTargetPoint&, NewLockOnPoint);

/**
 * A component that can be added to a PlayerController to rotate the Camera and Pawn to face the lock on point.
 */
UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class CRIMLOCKONSYSTEM_API ULockOnSystemComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	ULockOnSystemComponent();
	virtual void GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const override;
	virtual void PreNetReceive() override;
	virtual void BeginPlay() override;
	virtual void TickComponent(float DeltaTime, enum ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
	
	/** [Client + Server] When the lock on point changes. */
	UPROPERTY(BlueprintAssignable, DisplayName = "OnLockedOnPointChanged")
	FLockOnSystemLockOnPointSignature OnLockedOnPointChangedDelegate;

	/**
	 * Sets the camera lock to the new point.
	 * @param InLockOnPoint The point to lock onto.
	 */
	UFUNCTION(BlueprintCallable, Category = "LockOn System")
	void SetLockOnPoint(const FCrimTargetPoint& InLockOnPoint);
	
	/** Clears the lock on the target. */
	UFUNCTION(BlueprintCallable, Category = "LockOn System")
	void ClearLockOnPoint();
	
	/** Returns a const reference to the currently locked on point. */
	UFUNCTION(BlueprintPure, Category = "LockOn System")
	const FCrimTargetPoint& GetLockOnPoint() const { return LockOnPoint; }

protected:
	/** The maximum distance from a LockOnPoint that allows Locking on. */
	UPROPERTY(EditDefaultsOnly, Category = "LockOn System")
	float MaxLockOnRange = 2000.0f;

	/** Frequency to check if the target is in line of sight, within range, and is generally targetable. */
	UPROPERTY(EditDefaultsOnly, Category = "LockOn System")
	float CheckFrequency = 0.1f;
	
	/** The amount of time to break the LockOn when the Actor is too far away or obstructed behind an Object. */
	UPROPERTY(EditDefaultsOnly, Category = "LockOn System")
	float BreakLockOnDelay = 2.0f;

	/** Whether to accept control input when Locked on. */
	UPROPERTY(EditDefaultsOnly, Category = "LockOn System|Rotation")
	bool bIgnoreLookInput = true;

	/** The rate of rotation to face the LockOnPoint when IgnoreLookInput is false. */
	UPROPERTY(EditDefaultsOnly, Category = "LockOn System|Rotation", meta = (EditCondition="bIgnoreLookInput==false", ClampMin = 0.f))
	float CameraInterpSpeed = 2.0f;
	
	/**
	 * Setting this to true will tell the LockOn System to adjust the Pitch Offset (the Y axis) when locked on,
	 * depending on the distance to the target actor.
	 * It will ensure that the Camera will be moved up vertically the closer this Actor gets to its target.
	 * Formula:
	 * (DistanceToTarget * PitchDistanceCoefficient + PitchDistanceOffset) * -1.0f
	 * Then Clamped by PitchMin / PitchMax
	 */
	UPROPERTY(EditDefaultsOnly, Category = "LockOn System|Pitch Offset")
	bool bAdjustPitchBasedOnDistanceToTarget = true;

	UPROPERTY(EditDefaultsOnly, Category = "LockOn System|Pitch Offset")
	float PitchDistanceCoefficient = -0.2f;

	UPROPERTY(EditDefaultsOnly, Category = "LockOn System|Pitch Offset")
	float PitchDistanceOffset = 60.0f;

	UPROPERTY(EditDefaultsOnly, Category = "LockOn System|Pitch Offset")
	float PitchMin = -50.0f;

	UPROPERTY(EditDefaultsOnly, Category = "LockOn System|Pitch Offset")
	float PitchMax = -20.0f;
	
	virtual void OnRegister() override;
	
	virtual void OnLockOnPointChanged(const FCrimTargetPoint& OldLockOnPoint, const FCrimTargetPoint& NewLockOnPoint);
	
	UFUNCTION()
	virtual void OnPossessedPawnChanged(APawn* OldPawn, APawn* NewPawn);
	
	bool HasAuthority() const;
	
private:
	/** Cached value of whether our owner is a simulated Actor. */
	bool bCachedIsNetSimulated = false;
	void CacheIsNetSimulated();
	
	UPROPERTY()
	TObjectPtr<APlayerController> OwnerPlayerController;
	
	// The pawn controlled by the player controller.
	UPROPERTY()
	TObjectPtr<APawn> ControlledPawn;
	
	// The object of the controlled pawn that implements the interface. It may be the pawn or one of it's components.
	UPROPERTY()
	TObjectPtr<UObject> LockOnPawnInterface;
	
	// The currently locked onto actor/component.
	UPROPERTY(ReplicatedUsing = "OnRep_LockOnPoint")
	FCrimTargetPoint LockOnPoint;
	
	// The last LockOnPoint that was used.
	UPROPERTY()
	FCrimTargetPoint PreviousLockOnPoint;
	
	/** Functionality to clear the LockOnPoint if line of sight is broken or is not targetable. */
	FTimerHandle CheckLockOntPointTimerHandle;
	FTimerHandle BreakLockOnPointTimerHandle;
	bool bIsBreakingLineOfSight;
	
	void CheckLockOnPoint();
	bool ShouldBreakLockOn() const;
	void BreakLockOn();
	
	/** 
	 * Sets the control rotation on the owning player controller.
	 */
	void SetControlRotation(float DeltaTime) const;
	/** Gets the rotation for the camera to face the LockOnPoint */
	FRotator GetTargetControlRotation() const;
	
	UFUNCTION()
	void OnLockedOnPointOwnerDestroyed(AActor* DestroyedActor);
	
	UFUNCTION()
	void OnRep_LockOnPoint();
	
	void InitNewLockPoint(const FCrimTargetPoint& NewLockOnPoint);
	void DeinitOldLockOnPoint(const FCrimTargetPoint& OldLockOnPoint);
	
	UFUNCTION(Server, Reliable)
	void Server_SetLockOnPoint(const FCrimTargetPoint& InParams);
	
	UFUNCTION(Server, Reliable)
	void Server_ClearLockOnPoint();
};
