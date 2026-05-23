// Copyright Soccertitan 2026


#include "PlayerLockOnComponent.h"

#include "PawnTargetPointInterface.h"
#include "CrimTargetingSystemBlueprintFunctionLibrary.h"
#include "CrimTargetingSystemLogChannels.h"
#include "Net/UnrealNetwork.h"


UPlayerLockOnComponent::UPlayerLockOnComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
	SetIsReplicatedByDefault(true);
}

void UPlayerLockOnComponent::GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	
	FDoRepLifetimeParams Params;
	Params.bIsPushBased = true;
	Params.RepNotifyCondition = REPNOTIFY_OnChanged;
	DOREPLIFETIME_WITH_PARAMS_FAST(ThisClass, TargetPoint, Params);
}

void UPlayerLockOnComponent::PreNetReceive()
{
	Super::PreNetReceive();
	CacheIsNetSimulated();
}

void UPlayerLockOnComponent::BeginPlay()
{
	Super::BeginPlay();
	CacheIsNetSimulated();
	SetComponentTickEnabled(false);
	
	OwnerPlayerController = Cast<APlayerController>(GetOwner());
	if (!ensure(OwnerPlayerController))
	{
		UE_LOG(LogCrimTargetingSystem, Error, TEXT("[%s] PlayerLockOnComponent is meant to be added to PlayerControllers only."), *GetNameSafe(GetOwner()));
		Deactivate();
		return;
	}
	
	OnPossessedPawnChanged(nullptr, OwnerPlayerController->GetPawn());
	OwnerPlayerController->OnPossessedPawnChanged.AddUniqueDynamic(this, &UPlayerLockOnComponent::OnPossessedPawnChanged);
}

void UPlayerLockOnComponent::TickComponent(float DeltaTime, enum ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
	
	if (TargetPoint.IsValid() && ControlledPawn)
	{
		if (OwnerPlayerController->IsLocalController())
		{
			SetControlRotation(DeltaTime);
		}
	}
	else
	{
		ClearTargetPoint();
	}
}

void UPlayerLockOnComponent::SetTargetPoint(const FCrimTargetPoint& InTargetPoint)
{
	if (UCrimTargetingSystemBlueprintFunctionLibrary::IsTargetPointTargetable(OwnerPlayerController, InTargetPoint) &&
		TargetPoint != InTargetPoint)
	{
		PreviousTargetPoint = TargetPoint;
		TargetPoint = InTargetPoint;
		OnTargetPointChanged(PreviousTargetPoint, TargetPoint);
		
		if (!HasAuthority())
		{
			Server_SetTargetPoint(TargetPoint);
		}
		else
		{
			OnRep_TargetPoint();
		}
	}
	else
	{
		if (HasAuthority())
		{
			OnRep_TargetPoint();
		}
	}
}

void UPlayerLockOnComponent::ClearTargetPoint()
{
	if (TargetPoint.IsValid())
	{
		PreviousTargetPoint = TargetPoint;
		TargetPoint.Reset();
		OnTargetPointChanged(PreviousTargetPoint, TargetPoint);
		
		if (!HasAuthority())
		{
			Server_ClearTargetPoint();
		}
		else
		{
			OnRep_TargetPoint();
		}
	}
	else
	{
		if (HasAuthority())
		{
			OnRep_TargetPoint();
		}
	}
}

void UPlayerLockOnComponent::OnRegister()
{
	Super::OnRegister();
	CacheIsNetSimulated();
}

void UPlayerLockOnComponent::OnTargetPointChanged(const FCrimTargetPoint& OldTargetPoint, const FCrimTargetPoint& NewTargetPoint)
{
	OnTargetPointChangedDelegate.Broadcast(OldTargetPoint, NewTargetPoint);
	
	if (OldTargetPoint.IsValid())
	{
		DeinitOldTargetPoint(OldTargetPoint);
	}
	if (NewTargetPoint.IsValid())
	{
		InitNewTargetPoint(NewTargetPoint);
	}
}

void UPlayerLockOnComponent::OnRep_TargetPoint()
{
	if (!HasAuthority())
	{
		OnTargetPointChanged(PreviousTargetPoint, TargetPoint);
		PreviousTargetPoint = TargetPoint;
	}
}

void UPlayerLockOnComponent::InitNewTargetPoint(const FCrimTargetPoint& NewTargetPoint)
{
	SetComponentTickEnabled(true);
	
	if (const USceneComponent* StrongSceneComp = NewTargetPoint.SceneComponent.Get())
	{
		StrongSceneComp->GetOwner()->OnDestroyed.AddUniqueDynamic(this, &UPlayerLockOnComponent::OnTargetPointOwnerDestroyed);
	}
	else
	{
		NewTargetPoint.Actor.Get()->OnDestroyed.AddUniqueDynamic(this, &UPlayerLockOnComponent::OnTargetPointOwnerDestroyed);
	}
	
	GetWorld()->GetTimerManager().SetTimer(
		CheckTargetPointTimerHandle,
		this,
		&UPlayerLockOnComponent::CheckTargetPoint,
		CheckFrequency,
		true
	);
	
	if (bIgnoreLookInput)
	{
		OwnerPlayerController->SetIgnoreLookInput(true);
	}
	
	if (LockOnPawnInterface)
	{
		IPawnTargetPointInterface::Execute_SetTargetPoint(LockOnPawnInterface, NewTargetPoint);
	}
}

void UPlayerLockOnComponent::DeinitOldTargetPoint(const FCrimTargetPoint& OldTargetPoint)
{
	SetComponentTickEnabled(false);
	
	if (const USceneComponent* StrongSceneComp = OldTargetPoint.SceneComponent.Get())
	{
		StrongSceneComp->GetOwner()->OnDestroyed.RemoveAll(this);
	}
	else
	{
		OldTargetPoint.Actor.Get()->OnDestroyed.RemoveAll(this);
	}
	
	GetWorld()->GetTimerManager().ClearTimer(CheckTargetPointTimerHandle);
	
	if (bIgnoreLookInput)
	{
		OwnerPlayerController->SetIgnoreLookInput(false);
	}
	
	if (LockOnPawnInterface)
	{
		IPawnTargetPointInterface::Execute_SetTargetPoint(LockOnPawnInterface, FCrimTargetPoint());
	}
}

void UPlayerLockOnComponent::OnPossessedPawnChanged(APawn* OldPawn, APawn* NewPawn)
{
	if (TargetPoint.IsValid())
	{
		ClearTargetPoint();
	}
	ControlledPawn = NewPawn;
	
	if (NewPawn)
	{
		if (NewPawn->Implements<UPawnTargetPointInterface>())
		{
			LockOnPawnInterface = NewPawn;
		}
		else
		{
			LockOnPawnInterface = NewPawn->FindComponentByInterface(UPawnTargetPointInterface::StaticClass());
		}
	}
}

bool UPlayerLockOnComponent::HasAuthority() const
{
	return !bCachedIsNetSimulated;
}

void UPlayerLockOnComponent::CacheIsNetSimulated()
{
	bCachedIsNetSimulated = IsNetSimulating();
}

void UPlayerLockOnComponent::CheckTargetPoint()
{
	if (ShouldBreakLockOn() && !bIsBreakingLineOfSight)
	{
		bIsBreakingLineOfSight = true;
		GetWorld()->GetTimerManager().SetTimer(
			BreakTargetPointTimerHandle,
			this,
			&UPlayerLockOnComponent::BreakLockOn,
			BreakLockOnDelay
		);
	}
}

bool UPlayerLockOnComponent::ShouldBreakLockOn() const
{
	if (!UCrimTargetingSystemBlueprintFunctionLibrary::IsTargetPointTargetable(OwnerPlayerController, TargetPoint))
	{
		return true;
	}
	
	FHitResult HitResult;
	FCollisionQueryParams Params = FCollisionQueryParams(FName("LineTraceSingle"));
	Params.AddIgnoredActor(ControlledPawn);
	const FVector LockOnPointLocation = UCrimTargetingSystemBlueprintFunctionLibrary::GetTargetPointLocation(TargetPoint);
	
	bool bBlockedHit = GetWorld()->LineTraceSingleByChannel(
		HitResult,
		ControlledPawn->GetActorLocation(),
		LockOnPointLocation,
		ECC_Visibility,
		Params
	);
	
	if (bBlockedHit)
	{
		return true;
	}
	
	const float Distance = (ControlledPawn->GetActorLocation() - LockOnPointLocation).Size();
	if (Distance > MaxTargetingRange)
	{
		return true;
	}
	
	return false;
}

void UPlayerLockOnComponent::BreakLockOn()
{
	bIsBreakingLineOfSight = false;
	if (ShouldBreakLockOn())
	{
		ClearTargetPoint();
	}
}

void UPlayerLockOnComponent::SetControlRotation(float DeltaTime) const
{
	const FRotator TargetControlRotation = GetTargetControlRotation();
	
	if (bIgnoreLookInput)
	{
		OwnerPlayerController->SetControlRotation(TargetControlRotation);
	}
	else
	{
		const FRotator InterpTargetControlRotation = FMath::RInterpTo(OwnerPlayerController->GetControlRotation(), TargetControlRotation, DeltaTime, CameraInterpSpeed);
		OwnerPlayerController->SetControlRotation(InterpTargetControlRotation);
	}
}

FRotator UPlayerLockOnComponent::GetTargetControlRotation() const
{
	const FRotator ControlRotation = OwnerPlayerController->GetControlRotation();
	const FVector PawnLocation = ControlledPawn->GetActorLocation();
	const FVector LockOnPointLocation = UCrimTargetingSystemBlueprintFunctionLibrary::GetTargetPointLocation(TargetPoint);
	
	// Find look at rotation
	const FRotator LookRotation = FRotationMatrix::MakeFromX(LockOnPointLocation - PawnLocation).Rotator();
	const float Pitch = LookRotation.Pitch;
	float PitchOffset = 0.f;
	if (bAdjustPitchBasedOnDistanceToTarget)
	{
		const float DistanceToTarget = (PawnLocation - LockOnPointLocation).Size();
		const float PitchInRange = (DistanceToTarget * PitchDistanceCoefficient + PitchDistanceOffset) * -1.0f;
		PitchOffset = FMath::Clamp(PitchInRange, PitchMin, PitchMax);
	}
	
	return FRotator(Pitch + PitchOffset, LookRotation.Yaw, ControlRotation.Roll);
}

void UPlayerLockOnComponent::OnTargetPointOwnerDestroyed(AActor* DestroyedActor)
{
	ClearTargetPoint();
}

void UPlayerLockOnComponent::Server_SetTargetPoint_Implementation(const FCrimTargetPoint& InTargetPoint)
{
	SetTargetPoint(InTargetPoint);
}

void UPlayerLockOnComponent::Server_ClearTargetPoint_Implementation()
{
	ClearTargetPoint();
}

