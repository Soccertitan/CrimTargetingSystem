// Copyright Soccertitan 2026

#pragma once

#include "CoreMinimal.h"
#include "CrimTargetingTypes.h"
#include "Components/ActorComponent.h"
#include "PlayerLockOnComponent.generated.h"


class UTargetPointComponent;
class UCharacterMovementComponent;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FPlayerLockOnTargetPointSignature, const FCrimTargetPoint&, OldTargetPoint, const FCrimTargetPoint&, NewTargetPoint);

/**
 * A component that can be added to a PlayerController to rotate the view to the targeted point. If you want the pawn
 * to rotate towards the target, add the PawnTargetPointInterface to either the pawn or a component. It will call the function
 * automatically on the controlled pawn/component.
 */
UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class CRIMTARGETINGSYSTEM_API UPlayerLockOnComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UPlayerLockOnComponent();
	virtual void GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const override;
	virtual void PreNetReceive() override;
	virtual void BeginPlay() override;
	virtual void TickComponent(float DeltaTime, enum ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
	
	/** [Client + Server] When the TargetPoint changes. */
	UPROPERTY(BlueprintAssignable, DisplayName = "OnTargetPointChanged")
	FPlayerLockOnTargetPointSignature OnTargetPointChangedDelegate;

	/**
	 * Sets the camera lock to the new point.
	 * @param InTargetPoint The point to lock onto.
	 */
	UFUNCTION(BlueprintCallable, Category = "LockOn System")
	void SetTargetPoint(const FCrimTargetPoint& InTargetPoint);
	
	/** Clears the locked on target. */
	UFUNCTION(BlueprintCallable, Category = "LockOn System")
	void ClearTargetPoint();
	
	/** Returns a const reference to the currently targeted point. */
	UFUNCTION(BlueprintPure, Category = "LockOn System")
	const FCrimTargetPoint& GetTargetPoint() const { return TargetPoint; }

protected:
	/** The maximum distance from a TargetPoint that allows locking on. */
	UPROPERTY(EditDefaultsOnly, Category = "LockOn System")
	float MaxTargetingRange = 2000.0f;

	/** Frequency to check if the target is in line of sight, within range, and is generally targetable. */
	UPROPERTY(EditDefaultsOnly, Category = "LockOn System")
	float CheckFrequency = 0.1f;
	
	/** The amount of time to break the target when the Actor is too far away or obstructed behind an Object. */
	UPROPERTY(EditDefaultsOnly, Category = "LockOn System")
	float BreakLockOnDelay = 2.0f;

	/** Whether to accept control input when locked on. */
	UPROPERTY(EditDefaultsOnly, Category = "LockOn System|Rotation")
	bool bIgnoreLookInput = true;

	/** The rate of rotation to face the TargetPoint when IgnoreLookInput is false. */
	UPROPERTY(EditDefaultsOnly, Category = "LockOn System|Rotation", meta = (EditCondition="bIgnoreLookInput==false", ClampMin = 0.f))
	float CameraInterpSpeed = 4.0f;
	
	/**
	 * Setting this to true will tell the LockOn System to adjust the Pitch Offset (the Y axis) when locked on,
	 * depending on the distance to the target point.
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
	
	virtual void OnTargetPointChanged(const FCrimTargetPoint& OldTargetPoint, const FCrimTargetPoint& NewTargetPoint);
	
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
	
	// An object of the controlled pawn that implements the interface. It may be the pawn or one of its components.
	UPROPERTY()
	TObjectPtr<UObject> LockOnPawnInterface;
	
	// The currently locked onto actor/component.
	UPROPERTY(ReplicatedUsing = "OnRep_TargetPoint")
	FCrimTargetPoint TargetPoint;
	
	UPROPERTY()
	FCrimTargetPoint PreviousTargetPoint;
	
	/** Functionality to clear the TargetPoint if line of sight is broken or is not targetable. */
	FTimerHandle CheckTargetPointTimerHandle;
	FTimerHandle BreakTargetPointTimerHandle;
	bool bIsBreakingLineOfSight;
	
	void CheckTargetPoint();
	bool ShouldBreakLockOn() const;
	void BreakLockOn();
	
	/** 
	 * Sets the control rotation on the owning player controller.
	 */
	void SetControlRotation(float DeltaTime) const;
	/** Gets the rotation for the camera to face the TargetPoint */
	FRotator GetTargetControlRotation() const;
	
	UFUNCTION()
	void OnTargetPointOwnerDestroyed(AActor* DestroyedActor);
	
	UFUNCTION()
	void OnRep_TargetPoint();
	
	void InitNewTargetPoint(const FCrimTargetPoint& NewTargetPoint);
	void DeinitOldTargetPoint(const FCrimTargetPoint& OldTargetPoint);
	
	UFUNCTION(Server, Reliable)
	void Server_SetTargetPoint(const FCrimTargetPoint& InTargetPoint);
	
	UFUNCTION(Server, Reliable)
	void Server_ClearTargetPoint();
};
