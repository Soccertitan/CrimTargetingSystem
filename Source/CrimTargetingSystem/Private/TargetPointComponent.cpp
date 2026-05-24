// Copyright Soccertitan 2026


#include "TargetPointComponent.h"

#include "Net/UnrealNetwork.h"
#include "Net/Core/PushModel/PushModel.h"


UTargetPointComponent::UTargetPointComponent()
{
	PrimaryComponentTick.bCanEverTick = false;

	bHiddenInGame = true;
	SetIsReplicatedByDefault(true);
	SetCollisionProfileName(FName("Trigger"));
	SetCanEverAffectNavigation(false);

	SphereRadius = 0.f;
}

void UTargetPointComponent::GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	
	FDoRepLifetimeParams Params;
	Params.bIsPushBased = true;
	
	DOREPLIFETIME_WITH_PARAMS_FAST(ThisClass, LockOnPointTag, Params);
	DOREPLIFETIME_WITH_PARAMS_FAST(ThisClass, bTargetable, Params);
	DOREPLIFETIME_CONDITION_NOTIFY(ThisClass, CustomData, COND_None, REPNOTIFY_OnChanged);
}

void UTargetPointComponent::PreNetReceive()
{
	Super::PreNetReceive();
	CacheIsNetSimulated();
}

void UTargetPointComponent::OnRegister()
{
	Super::OnRegister();
	CacheIsNetSimulated();
}

void UTargetPointComponent::BeginPlay()
{
	Super::BeginPlay();
	CacheIsNetSimulated();
}

void UTargetPointComponent::SetLockOnPointTag(FGameplayTag& InLockOnPointTag)
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

bool UTargetPointComponent::IsTargetPointTargetable_Implementation(AController* SourceController)
{
	return bTargetable;
}

void UTargetPointComponent::SetIsTargetable(const bool bEnable)
{
	if (HasAuthority() && bTargetable != bEnable)
	{
		bTargetable = bEnable;
		MARK_PROPERTY_DIRTY_FROM_NAME(ThisClass, bTargetable, this);
	}
}

void UTargetPointComponent::SetCustomData(const FInstancedStruct& InCustomData)
{
	if (HasAuthority() && InCustomData.IsValid())
	{
		CustomData = InCustomData;
		OnRep_CustomData();
		OnCustomDataSet();
	}
}

void UTargetPointComponent::OnRep_CustomData()
{
	OnCustomDataSet();
}

bool UTargetPointComponent::HasAuthority() const
{
	return !bCachedIsNetSimulated;
}

void UTargetPointComponent::CacheIsNetSimulated()
{
	bCachedIsNetSimulated = IsNetSimulating();
}
