// Copyright Soccertitan 2026


#include "LockOnSystemComponent.h"

#include "LockOnPawnInterface.h"
#include "LockOnSystemBlueprintFunctionLibrary.h"
#include "LockOnSystemLogChannels.h"
#include "Net/UnrealNetwork.h"


ULockOnSystemComponent::ULockOnSystemComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
	SetIsReplicatedByDefault(true);
}

void ULockOnSystemComponent::GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	
	FDoRepLifetimeParams Params;
	Params.bIsPushBased = true;
	Params.RepNotifyCondition = REPNOTIFY_OnChanged;
	DOREPLIFETIME_WITH_PARAMS_FAST(ThisClass, LockOnPoint, Params);
}

void ULockOnSystemComponent::PreNetReceive()
{
	Super::PreNetReceive();
	CacheIsNetSimulated();
}

void ULockOnSystemComponent::BeginPlay()
{
	Super::BeginPlay();
	CacheIsNetSimulated();
	SetComponentTickEnabled(false);
	
	OwnerPlayerController = Cast<APlayerController>(GetOwner());
	if (!ensure(OwnerPlayerController))
	{
		UE_LOG(LogLockOnSystem, Error, TEXT("[%s] LockOnSystemComponent is meant to be added to PlayerControllers only."), *GetNameSafe(GetOwner()));
		Deactivate();
		return;
	}
	
	OnPossessedPawnChanged(nullptr, OwnerPlayerController->GetPawn());
	OwnerPlayerController->OnPossessedPawnChanged.AddUniqueDynamic(this, &ULockOnSystemComponent::OnPossessedPawnChanged);
}

void ULockOnSystemComponent::TickComponent(float DeltaTime, enum ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
	
	if (LockOnPoint.IsValid() && ControlledPawn)
	{
		if (OwnerPlayerController->IsLocalController())
		{
			SetControlRotation(DeltaTime);
		}
	}
	else
	{
		ClearLockOnPoint();
	}
}

void ULockOnSystemComponent::SetLockOnPoint(const FCrimTargetPoint& InLockOnPoint)
{
	if (ULockOnSystemBlueprintFunctionLibrary::IsLockOnPointTargetable(OwnerPlayerController, InLockOnPoint) &&
		LockOnPoint != InLockOnPoint)
	{
		PreviousLockOnPoint = LockOnPoint;
		LockOnPoint = InLockOnPoint;
		OnLockOnPointChanged(PreviousLockOnPoint, LockOnPoint);
		
		if (!HasAuthority())
		{
			Server_SetLockOnPoint(LockOnPoint);
		}
		else
		{
			OnRep_LockOnPoint();
		}
	}
	else
	{
		if (HasAuthority())
		{
			OnRep_LockOnPoint();
		}
	}
}

void ULockOnSystemComponent::ClearLockOnPoint()
{
	if (LockOnPoint.IsValid())
	{
		PreviousLockOnPoint = LockOnPoint;
		LockOnPoint.Reset();
		OnLockOnPointChanged(PreviousLockOnPoint, LockOnPoint);
		
		if (!HasAuthority())
		{
			Server_ClearLockOnPoint();
		}
		else
		{
			OnRep_LockOnPoint();
		}
	}
	else
	{
		if (HasAuthority())
		{
			OnRep_LockOnPoint();
		}
	}
}

void ULockOnSystemComponent::OnRegister()
{
	Super::OnRegister();
	CacheIsNetSimulated();
}

void ULockOnSystemComponent::OnLockOnPointChanged(const FCrimTargetPoint& OldLockOnPoint, const FCrimTargetPoint& NewLockOnPoint)
{
	OnLockedOnPointChangedDelegate.Broadcast(OldLockOnPoint, NewLockOnPoint);
	
	if (OldLockOnPoint.IsValid())
	{
		DeinitOldLockOnPoint(OldLockOnPoint);
	}
	if (NewLockOnPoint.IsValid())
	{
		InitNewLockPoint(NewLockOnPoint);
	}
}

void ULockOnSystemComponent::OnRep_LockOnPoint()
{
	if (!HasAuthority())
	{
		OnLockOnPointChanged(PreviousLockOnPoint, LockOnPoint);
		PreviousLockOnPoint = LockOnPoint;
	}
}

void ULockOnSystemComponent::InitNewLockPoint(const FCrimTargetPoint& NewLockOnPoint)
{
	SetComponentTickEnabled(true);
	
	if (const USceneComponent* StrongSceneComp = NewLockOnPoint.SceneComponent.Get())
	{
		StrongSceneComp->GetOwner()->OnDestroyed.AddUniqueDynamic(this, &ULockOnSystemComponent::OnLockedOnPointOwnerDestroyed);
	}
	else
	{
		NewLockOnPoint.Actor.Get()->OnDestroyed.AddUniqueDynamic(this, &ULockOnSystemComponent::OnLockedOnPointOwnerDestroyed);
	}
	
	GetWorld()->GetTimerManager().SetTimer(
		CheckLockOntPointTimerHandle,
		this,
		&ULockOnSystemComponent::CheckLockOnPoint,
		CheckFrequency,
		true
	);
	
	if (bIgnoreLookInput)
	{
		OwnerPlayerController->SetIgnoreLookInput(true);
	}
	
	if (LockOnPawnInterface)
	{
		ILockOnPawnInterface::Execute_SetLockOnPoint(LockOnPawnInterface, NewLockOnPoint);
	}
}

void ULockOnSystemComponent::DeinitOldLockOnPoint(const FCrimTargetPoint& OldLockOnPoint)
{
	SetComponentTickEnabled(false);
	
	if (const USceneComponent* StrongSceneComp = OldLockOnPoint.SceneComponent.Get())
	{
		StrongSceneComp->GetOwner()->OnDestroyed.RemoveAll(this);
	}
	else
	{
		OldLockOnPoint.Actor.Get()->OnDestroyed.RemoveAll(this);
	}
	
	GetWorld()->GetTimerManager().ClearTimer(CheckLockOntPointTimerHandle);
	
	if (bIgnoreLookInput)
	{
		OwnerPlayerController->SetIgnoreLookInput(false);
	}
	
	if (LockOnPawnInterface)
	{
		ILockOnPawnInterface::Execute_SetLockOnPoint(LockOnPawnInterface, FCrimTargetPoint());
	}
}

void ULockOnSystemComponent::OnPossessedPawnChanged(APawn* OldPawn, APawn* NewPawn)
{
	if (LockOnPoint.IsValid())
	{
		ClearLockOnPoint();
	}
	ControlledPawn = NewPawn;
	
	if (NewPawn)
	{
		if (NewPawn->Implements<ULockOnPawnInterface>())
		{
			LockOnPawnInterface = NewPawn;
		}
		else
		{
			LockOnPawnInterface = NewPawn->FindComponentByInterface(ULockOnPawnInterface::StaticClass());
		}
	}
}

bool ULockOnSystemComponent::HasAuthority() const
{
	return !bCachedIsNetSimulated;
}

void ULockOnSystemComponent::CacheIsNetSimulated()
{
	bCachedIsNetSimulated = IsNetSimulating();
}

void ULockOnSystemComponent::CheckLockOnPoint()
{
	if (ShouldBreakLockOn() && !bIsBreakingLineOfSight)
	{
		bIsBreakingLineOfSight = true;
		GetWorld()->GetTimerManager().SetTimer(
			BreakLockOnPointTimerHandle,
			this,
			&ULockOnSystemComponent::BreakLockOn,
			BreakLockOnDelay
		);
	}
}

bool ULockOnSystemComponent::ShouldBreakLockOn() const
{
	if (!ULockOnSystemBlueprintFunctionLibrary::IsLockOnPointTargetable(OwnerPlayerController, LockOnPoint))
	{
		return true;
	}
	
	FHitResult HitResult;
	FCollisionQueryParams Params = FCollisionQueryParams(FName("LineTraceSingle"));
	Params.AddIgnoredActor(ControlledPawn);
	const FVector LockOnPointLocation = ULockOnSystemBlueprintFunctionLibrary::GetLockOnPointLocation(LockOnPoint);
	
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
	if (Distance > MaxLockOnRange)
	{
		return true;
	}
	
	return false;
}

void ULockOnSystemComponent::BreakLockOn()
{
	bIsBreakingLineOfSight = false;
	if (ShouldBreakLockOn())
	{
		ClearLockOnPoint();
	}
}

void ULockOnSystemComponent::SetControlRotation(float DeltaTime) const
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

FRotator ULockOnSystemComponent::GetTargetControlRotation() const
{
	const FRotator ControlRotation = OwnerPlayerController->GetControlRotation();
	const FVector PawnLocation = ControlledPawn->GetActorLocation();
	const FVector LockOnPointLocation = ULockOnSystemBlueprintFunctionLibrary::GetLockOnPointLocation(LockOnPoint);
	
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

void ULockOnSystemComponent::OnLockedOnPointOwnerDestroyed(AActor* DestroyedActor)
{
	ClearLockOnPoint();
}

void ULockOnSystemComponent::Server_SetLockOnPoint_Implementation(const FCrimTargetPoint& InParams)
{
	SetLockOnPoint(InParams);
}

void ULockOnSystemComponent::Server_ClearLockOnPoint_Implementation()
{
	ClearLockOnPoint();
}

