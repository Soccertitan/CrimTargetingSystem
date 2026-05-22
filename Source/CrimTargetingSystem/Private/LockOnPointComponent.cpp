// Copyright Soccertitan 2026


#include "LockOnPointComponent.h"

#include "Net/UnrealNetwork.h"
#include "Net/Core/PushModel/PushModel.h"


ULockOnPointComponent::ULockOnPointComponent()
{
	PrimaryComponentTick.bCanEverTick = false;

	bHiddenInGame = true;
	SetIsReplicatedByDefault(true);
	SetCollisionProfileName(FName("Trigger"));
	SetCanEverAffectNavigation(false);

	SphereRadius = 0.f;
}

void ULockOnPointComponent::GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	
	FDoRepLifetimeParams Params;
	Params.bIsPushBased = true;
	
	DOREPLIFETIME_WITH_PARAMS_FAST(ThisClass, LockOnPointTag, Params);
	DOREPLIFETIME_WITH_PARAMS_FAST(ThisClass, bTargetable, Params);
	DOREPLIFETIME_CONDITION_NOTIFY(ThisClass, CustomData, COND_None, REPNOTIFY_OnChanged);
}

void ULockOnPointComponent::PreNetReceive()
{
	Super::PreNetReceive();
	CacheIsNetSimulated();
}

void ULockOnPointComponent::OnRegister()
{
	Super::OnRegister();
	CacheIsNetSimulated();
}

void ULockOnPointComponent::BeginPlay()
{
	Super::BeginPlay();
	CacheIsNetSimulated();
}

void ULockOnPointComponent::SetLockOnPointTag(FGameplayTag& InLockOnPointTag)
{
	if (HasAuthority() && InLockOnPointTag.IsValid())
	{
		if (LockOnPointTag != InLockOnPointTag)
		{
			LockOnPointTag = InLockOnPointTag;
			MARK_PROPERTY_DIRTY_FROM_NAME(ThisClass, LockOnPointTag, this);
		}
	}
}

bool ULockOnPointComponent::IsLockOnPointTargetable_Implementation(APlayerController* SourcePlayerController)
{
	return bTargetable;
}

void ULockOnPointComponent::SetIsTargetable(const bool bEnable)
{
	if (HasAuthority() && bTargetable != bEnable)
	{
		bTargetable = bEnable;
		MARK_PROPERTY_DIRTY_FROM_NAME(ThisClass, bTargetable, this);
	}
}

void ULockOnPointComponent::SetCustomData(const FInstancedStruct& InCustomData)
{
	if (HasAuthority() && InCustomData.IsValid())
	{
		CustomData = InCustomData;
		OnRep_CustomData();
		OnCustomDataSet();
	}
}

void ULockOnPointComponent::OnRep_CustomData()
{
	OnCustomDataSet();
}

bool ULockOnPointComponent::HasAuthority() const
{
	return !bCachedIsNetSimulated;
}

void ULockOnPointComponent::CacheIsNetSimulated()
{
	bCachedIsNetSimulated = IsNetSimulating();
}
