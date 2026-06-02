// Copyright Soccertitan 2026


#include "CrimTargetingSystemComponent.h"

#include "PawnTargetingSystemInterface.h"
#include "CrimTargetingSystemBlueprintFunctionLibrary.h"
#include "Net/UnrealNetwork.h"
#include "Net/Core/PushModel/PushModel.h"


UCrimTargetingSystemComponent::UCrimTargetingSystemComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
	SetIsReplicatedByDefault(true);
}

void UCrimTargetingSystemComponent::GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	
	FDoRepLifetimeParams Params;
	Params.bIsPushBased = true;
	Params.RepNotifyCondition = REPNOTIFY_OnChanged;
	DOREPLIFETIME_WITH_PARAMS_FAST(ThisClass, TargetPoint, Params);
	DOREPLIFETIME_WITH_PARAMS_FAST(ThisClass, bLockedOn, Params);
}

void UCrimTargetingSystemComponent::PreNetReceive()
{
	Super::PreNetReceive();
	CacheIsNetSimulated();
}

void UCrimTargetingSystemComponent::BeginPlay()
{
	Super::BeginPlay();
	CacheIsNetSimulated();
	SetComponentTickEnabled(false);
	
	APlayerController* PC = Cast<APlayerController>(GetOwner());
	if (PC)
	{
		SetController(PC);
	}
}

void UCrimTargetingSystemComponent::TickComponent(float DeltaTime, enum ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
	
	if (TargetPoint.IsValid() && Controller)
	{
		if (bLockedOn)
		{
			SetControlRotation(DeltaTime);
		}
	}
	else
	{
		ClearTargetPoint();
	}
}

void UCrimTargetingSystemComponent::SetTargetPoint(const FCrimTargetPoint& InTargetPoint)
{
	if (InTargetPoint != TargetPoint && InTargetPoint.IsTargetable(Controller))
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
			MARK_PROPERTY_DIRTY_FROM_NAME(ThisClass, TargetPoint, this);
		}
	}
	else if (HasAuthority())
	{
		MARK_PROPERTY_DIRTY_FROM_NAME(ThisClass, TargetPoint, this);
	}
}

void UCrimTargetingSystemComponent::ClearTargetPoint()
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
			MARK_PROPERTY_DIRTY_FROM_NAME(ThisClass, TargetPoint, this);
		}
	}
	else if (HasAuthority())
	{
		MARK_PROPERTY_DIRTY_FROM_NAME(ThisClass, TargetPoint, this);
	}
}

void UCrimTargetingSystemComponent::SetLockOnState(const bool bEnable)
{
	if (bEnable != bLockedOn)
	{
		if (bEnable && TargetPoint.IsValid() ||
			bEnable == false)
		{
			bLockedOn = bEnable;
			OnLockOnStateChanged();
			
			if (!HasAuthority())
			{
				Server_SetLockOnState(bEnable);
			}
			else
			{
				MARK_PROPERTY_DIRTY_FROM_NAME(ThisClass, bLockedOn, this);
			}
		}
	}
	else if (HasAuthority())
	{
		MARK_PROPERTY_DIRTY_FROM_NAME(ThisClass, bLockedOn, this);
	}
}

void UCrimTargetingSystemComponent::SetController(AController* PC)
{
	if (PC != Controller)
	{
		if (Controller)
		{
			SetLockOnState(false);
			OnPossessedPawnChanged(ControlledPawn, nullptr);
			Controller->OnPossessedPawnChanged.RemoveAll(this);
		}
		
		Controller = PC;
		
		if (Controller)
		{
			OnPossessedPawnChanged(nullptr, Controller->GetPawn());
			Controller->OnPossessedPawnChanged.AddUniqueDynamic(this, &UCrimTargetingSystemComponent::OnPossessedPawnChanged);
		}
	}
}

void UCrimTargetingSystemComponent::OnRegister()
{
	Super::OnRegister();
	CacheIsNetSimulated();
}

void UCrimTargetingSystemComponent::OnTargetPointChanged(const FCrimTargetPoint& OldTargetPoint, const FCrimTargetPoint& NewTargetPoint)
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
	else
	{
		SetLockOnState(false);
	}
	
	if (PawnTargetingInterface)
	{
		IPawnTargetingSystemInterface::Execute_SetTargetPoint(PawnTargetingInterface, NewTargetPoint);
	}
}

void UCrimTargetingSystemComponent::OnLockOnStateChanged()
{
	OnLockOnStateChangedDelegate.Broadcast(bLockedOn);
	
	if (bIgnoreLookInput && Controller)
	{
		Controller->SetIgnoreLookInput(bLockedOn);
	}
	if (PawnTargetingInterface)
	{
		IPawnTargetingSystemInterface::Execute_SetLockOnState(PawnTargetingInterface, bLockedOn);
	}
}

void UCrimTargetingSystemComponent::OnRep_TargetPoint()
{
	OnTargetPointChanged(PreviousTargetPoint, TargetPoint);
	PreviousTargetPoint = TargetPoint;
}

void UCrimTargetingSystemComponent::OnRep_LockedOn()
{
	OnLockOnStateChanged();
}

void UCrimTargetingSystemComponent::InitNewTargetPoint(const FCrimTargetPoint& NewTargetPoint)
{
	SetComponentTickEnabled(true);
	
	if (AActor* OwnerActor = NewTargetPoint.GetActor())
	{
		OwnerActor->OnDestroyed.AddUniqueDynamic(this, &UCrimTargetingSystemComponent::OnTargetPointOwnerDestroyed);
	}
	
	GetWorld()->GetTimerManager().SetTimer(
		CheckTargetPointTimerHandle,
		this,
		&UCrimTargetingSystemComponent::CheckTargetPoint,
		CheckFrequency.GetValue(),
		true
	);
}

void UCrimTargetingSystemComponent::DeinitOldTargetPoint(const FCrimTargetPoint& OldTargetPoint)
{
	SetComponentTickEnabled(false);
	
	if (AActor* OwnerActor = OldTargetPoint.GetActor())
	{
		OwnerActor->OnDestroyed.RemoveAll(this);
	}
	
	GetWorld()->GetTimerManager().ClearTimer(CheckTargetPointTimerHandle);
	
	if (PawnTargetingInterface)
	{
		IPawnTargetingSystemInterface::Execute_SetTargetPoint(PawnTargetingInterface, FCrimTargetPoint());
	}
}

void UCrimTargetingSystemComponent::OnPossessedPawnChanged(APawn* OldPawn, APawn* NewPawn)
{
	if (TargetPoint.IsValid())
	{
		ClearTargetPoint();
	}
	ControlledPawn = NewPawn;
	PawnTargetingInterface = nullptr;
	
	if (NewPawn)
	{
		PawnTargetingInterface = NewPawn->FindComponentByInterface(UPawnTargetingSystemInterface::StaticClass());
		if (PawnTargetingInterface == nullptr && NewPawn->Implements<UPawnTargetingSystemInterface>())
		{
			PawnTargetingInterface = NewPawn;
		}
	}
}

bool UCrimTargetingSystemComponent::HasAuthority() const
{
	return !bCachedIsNetSimulated;
}

void UCrimTargetingSystemComponent::CacheIsNetSimulated()
{
	bCachedIsNetSimulated = IsNetSimulating();
}

void UCrimTargetingSystemComponent::CheckTargetPoint()
{
	if (ShouldBreakTargeting() && !bIsBreakingLineOfSight)
	{
		bIsBreakingLineOfSight = true;
		GetWorld()->GetTimerManager().SetTimer(
			BreakTargetPointTimerHandle,
			this,
			&UCrimTargetingSystemComponent::BreakTargeting,
			BreakTargetingDelay.GetValue()
		);
	}
}

bool UCrimTargetingSystemComponent::ShouldBreakTargeting() const
{
	if (!UCrimTargetingSystemBlueprintFunctionLibrary::IsTargetPointTargetable(Controller, TargetPoint))
	{
		return true;
	}
	
	FMinimalViewInfo ViewInfo;
	GetViewInfo(ViewInfo);
	FHitResult HitResult;
	FCollisionQueryParams Params = FCollisionQueryParams(FName("LineTraceSingle"), bComplexTrace);
	Params.AddIgnoredActor(ControlledPawn);
	const ECollisionChannel CollisionChannel = UEngineTypes::ConvertToCollisionChannel(TraceChannel);
	const FVector LockOnPointLocation = UCrimTargetingSystemBlueprintFunctionLibrary::GetTargetPointLocation(TargetPoint);
	
	bool bBlockedHit = GetWorld()->LineTraceSingleByChannel(
		HitResult,
		ViewInfo.Location,
		LockOnPointLocation,
		CollisionChannel,
		Params
	);
	
	if (bBlockedHit)
	{
		return true;
	}
	
	const float Distance = (ControlledPawn->GetActorLocation() - LockOnPointLocation).Size();
	if (Distance > MaxTargetingRange.GetValue())
	{
		return true;
	}
	
	return false;
}

void UCrimTargetingSystemComponent::GetViewInfo(FMinimalViewInfo& OutViewInfo) const
{
	OutViewInfo = DefaultViewInfo;
	if (ControlledPawn)
	{
		if (bUseActorsEyes)
		{
			ControlledPawn->GetActorEyesViewPoint(OutViewInfo.Location, OutViewInfo.Rotation);
		}
		else
		{
			ControlledPawn->CalcCamera(0.f, OutViewInfo);
		}
	}
}

void UCrimTargetingSystemComponent::BreakTargeting()
{
	bIsBreakingLineOfSight = false;
	if (ShouldBreakTargeting())
	{
		ClearTargetPoint();
	}
}

void UCrimTargetingSystemComponent::SetControlRotation(float DeltaTime) const
{
	const FRotator TargetControlRotation = GetTargetControlRotation();
	
	const FRotator InterpTargetControlRotation = FMath::RInterpTo(
		Controller->GetControlRotation(), 
		TargetControlRotation, DeltaTime, CameraInterpSpeed.GetValue());
	Controller->SetControlRotation(InterpTargetControlRotation);
}

FRotator UCrimTargetingSystemComponent::GetTargetControlRotation() const
{
	const FRotator ControlRotation = Controller->GetControlRotation();
	const FVector PawnLocation = ControlledPawn->GetActorLocation();
	const FVector LockOnPointLocation = UCrimTargetingSystemBlueprintFunctionLibrary::GetTargetPointLocation(TargetPoint);
	
	// Find look at rotation
	const FRotator LookRotation = FRotationMatrix::MakeFromX(LockOnPointLocation - PawnLocation).Rotator();
	const float Pitch = LookRotation.Pitch;
	float PitchOffset = 0.f;
	if (bAdjustPitchBasedOnDistanceToTarget)
	{
		const float DistanceToTarget = (PawnLocation - LockOnPointLocation).Size();
		const float PitchInRange = (DistanceToTarget * PitchDistanceCoefficient.GetValue() + PitchDistanceOffset.GetValue()) * -1.0f;
		PitchOffset = FMath::Clamp(PitchInRange, PitchMin.GetValue(), PitchMax.GetValue());
	}
	
	return FRotator(Pitch + PitchOffset, LookRotation.Yaw, ControlRotation.Roll);
}

void UCrimTargetingSystemComponent::OnTargetPointOwnerDestroyed(AActor* DestroyedActor)
{
	ClearTargetPoint();
}

void UCrimTargetingSystemComponent::Server_SetTargetPoint_Implementation(const FCrimTargetPoint& InTargetPoint)
{
	SetTargetPoint(InTargetPoint);
}

void UCrimTargetingSystemComponent::Server_ClearTargetPoint_Implementation()
{
	ClearTargetPoint();
}

void UCrimTargetingSystemComponent::Server_SetLockOnState_Implementation(bool bEnable)
{
	SetLockOnState(bEnable);
}
