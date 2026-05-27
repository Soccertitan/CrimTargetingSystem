// Copyright Soccertitan 2026

#pragma once

#include "CoreMinimal.h"
#include "CrimTargetingTypes.h"
#include "ScalableFloat.h"
#include "Components/ActorComponent.h"
#include "CrimTargetingSystemComponent.generated.h"


class UTargetPointComponent;
class UCharacterMovementComponent;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FCrimTargetingSystemTargetPointSignature, const FCrimTargetPoint&, OldTargetPoint, const FCrimTargetPoint&, NewTargetPoint);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FCrimTargetingSystemLockOnStateSignature, bool, bLockedOn);

/**
 * A component that can will rotate the view to the targeted point. If you want the pawn to rotate towards the target, 
 * add the PawnTargetPointInterface to either the pawn or a component on the pawn. It will call the function
 * automatically on the controlled pawn/component.
 */
UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class CRIMTARGETINGSYSTEM_API UCrimTargetingSystemComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UCrimTargetingSystemComponent();
	virtual void GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const override;
	virtual void PreNetReceive() override;
	virtual void BeginPlay() override;
	virtual void TickComponent(float DeltaTime, enum ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
	
	/** [Client + Server] When the TargetPoint changes. */
	UPROPERTY(BlueprintAssignable, DisplayName = "OnTargetPointChanged")
	FCrimTargetingSystemTargetPointSignature OnTargetPointChangedDelegate;
	
	/** [Client + Server] When locked on or not onto a target. */
	UPROPERTY(BlueprintAssignable, DisplayName = "OnLockOnStateChanged")
	FCrimTargetingSystemLockOnStateSignature OnLockOnStateChangedDelegate;

	/**
	 * Sets the camera lock to the new point.
	 * @param InTargetPoint The point to lock onto.
	 */
	UFUNCTION(BlueprintCallable, Category = "Targeting System")
	void SetTargetPoint(const FCrimTargetPoint& InTargetPoint);
	
	/** Clears the locked on target. */
	UFUNCTION(BlueprintCallable, Category = "Targeting System")
	void ClearTargetPoint();
	
	/** Returns a const reference to the currently targeted point. */
	UFUNCTION(BlueprintPure, Category = "Targeting System")
	const FCrimTargetPoint& GetTargetPoint() const { return TargetPoint; }
	
	/** Set to True to enable lock on and false to disable. Locking on causes the camera to rotate towards the target point. */
	UFUNCTION(BlueprintCallable, Category = "Targeting System")
	void SetLockOnState(const bool bEnable);
	
	UFUNCTION(BlueprintPure, Category = "Targeting System")
	bool IsLockedOn() const { return bLockedOn; }
	
	/** Call this if the component is attached to a non PC actor. */
	UFUNCTION(BlueprintCallable, Category = "Targeting System")
	void SetPlayerController(APlayerController* PC);

protected:
	/** The maximum distance from a TargetPoint that allows locking on. */
	UPROPERTY(EditDefaultsOnly, Category = "Targeting System")
	FScalableFloat MaxTargetingRange = 2000.0f;

	/** Frequency to check if the target is in line of sight, within range, and is generally targetable. */
	UPROPERTY(EditDefaultsOnly, Category = "Targeting System")
	FScalableFloat CheckFrequency = 0.1f;
	
	/** The amount of time to break the target when the Actor is too far away or obstructed behind an Object. */
	UPROPERTY(EditDefaultsOnly, Category = "Targeting System")
	FScalableFloat BreakTargetingDelay = 2.0f;

	/** Whether to accept control input when locked on. */
	UPROPERTY(EditDefaultsOnly, Category = "Targeting System|Rotation")
	bool bIgnoreLookInput = true;

	/** The rate of rotation to face the TargetPoint. */
	UPROPERTY(EditDefaultsOnly, Category = "Targeting System|Rotation", meta = (ClampMin = 0.f))
	FScalableFloat CameraInterpSpeed = 0.0f;
	
	/**
	 * Setting this to true will tell the Targeting System to adjust the Pitch Offset (the Y axis) when locked on,
	 * depending on the distance to the target point.
	 * It will ensure that the Camera will be moved up vertically the closer this Actor gets to its target.
	 * Formula:
	 * (DistanceToTarget * PitchDistanceCoefficient + PitchDistanceOffset) * -1.0f
	 * Then Clamped by PitchMin / PitchMax
	 */
	UPROPERTY(EditDefaultsOnly, Category = "Targeting System|Pitch Offset")
	bool bAdjustPitchBasedOnDistanceToTarget = true;

	UPROPERTY(EditDefaultsOnly, Category = "Targeting System|Pitch Offset")
	FScalableFloat PitchDistanceCoefficient = -0.2f;

	UPROPERTY(EditDefaultsOnly, Category = "Targeting System|Pitch Offset")
	FScalableFloat PitchDistanceOffset = 60.0f;

	UPROPERTY(EditDefaultsOnly, Category = "Targeting System|Pitch Offset")
	FScalableFloat PitchMin = -50.0f;

	UPROPERTY(EditDefaultsOnly, Category = "Targeting System|Pitch Offset")
	FScalableFloat PitchMax = -20.0f;
	
	virtual void OnRegister() override;
	
	virtual void OnTargetPointChanged(const FCrimTargetPoint& OldTargetPoint, const FCrimTargetPoint& NewTargetPoint);
	virtual void OnLockOnStateChanged();
	
	UFUNCTION()
	virtual void OnPossessedPawnChanged(APawn* OldPawn, APawn* NewPawn);
	
	/** Gets the rotation for the camera to face the TargetPoint */
	virtual FRotator GetTargetControlRotation() const;
	/** Returns true if the TargetPoint should be cleared. */
	virtual bool ShouldBreakTargeting() const;
	
	bool HasAuthority() const;
	
private:
	/** Cached value of whether our owner is a simulated Actor. */
	bool bCachedIsNetSimulated = false;
	void CacheIsNetSimulated();
	
	UPROPERTY()
	TObjectPtr<AController> Controller;
	
	// The pawn controlled by the player controller.
	UPROPERTY()
	TObjectPtr<APawn> ControlledPawn;
	
	// A component of the controlled pawn (or pawn itself) that implements the PawnTargetingSystemInterface.
	UPROPERTY()
	TObjectPtr<UObject> PawnTargetingInterface;
	
	// The currently locked onto actor/component.
	UPROPERTY(ReplicatedUsing = "OnRep_TargetPoint")
	FCrimTargetPoint TargetPoint;
	
	UPROPERTY(ReplicatedUsing = "OnRep_LockedOn")
	bool bLockedOn = false;
	
	UPROPERTY()
	FCrimTargetPoint PreviousTargetPoint;
	
	/** Functionality to clear the TargetPoint if line of sight is broken or is not targetable. */
	FTimerHandle CheckTargetPointTimerHandle;
	FTimerHandle BreakTargetPointTimerHandle;
	bool bIsBreakingLineOfSight;
	
	void CheckTargetPoint();
	void BreakTargeting();
	
	/** 
	 * Sets the control rotation on the owning player controller.
	 */
	void SetControlRotation(float DeltaTime) const;
	
	UFUNCTION()
	void OnTargetPointOwnerDestroyed(AActor* DestroyedActor);
	
	UFUNCTION()
	void OnRep_TargetPoint();
	UFUNCTION()
	void OnRep_LockedOn();
	
	void InitNewTargetPoint(const FCrimTargetPoint& NewTargetPoint);
	void DeinitOldTargetPoint(const FCrimTargetPoint& OldTargetPoint);
	
	UFUNCTION(Server, Reliable)
	void Server_SetTargetPoint(const FCrimTargetPoint& InTargetPoint);
	
	UFUNCTION(Server, Reliable)
	void Server_ClearTargetPoint();
	
	UFUNCTION(Server, Reliable)
	void Server_SetLockOnState(bool bEnable);
};
