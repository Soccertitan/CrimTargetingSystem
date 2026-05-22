// Copyright Soccertitan 2026


#include "LockOnPointManagerComponent.h"

#include "LockOnPointComponent.h"


ULockOnPointManagerComponent::ULockOnPointManagerComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
	SetIsReplicatedByDefault(false);
}

void ULockOnPointManagerComponent::BeginPlay()
{
	Super::BeginPlay();

	InitLockOnPoints();	
}

void ULockOnPointManagerComponent::SetTargetPointEnabledByTag(const FGameplayTag& Tag, bool bEnable)
{
	for (TObjectPtr<ULockOnPointComponent>& Component : LockOnPointComponents)
	{
		if (Component && Component->GetLockOnPointTag().MatchesTag(Tag))
		{
			Component->SetIsTargetable(bEnable);
		}
	}
}

void ULockOnPointManagerComponent::SetLockOnPointCustomDataByTag(const FGameplayTag& Tag, const FInstancedStruct& CustomData)
{
	if (CustomData.IsValid())
	{
		for (TObjectPtr<ULockOnPointComponent>& Component : LockOnPointComponents)
		{
			if (Component && Component->GetLockOnPointTag().MatchesTag(Tag))
			{
				Component->SetCustomData(CustomData);
			}
		}
	}
}

void ULockOnPointManagerComponent::InitLockOnPoints()
{
	if (IsNetSimulating())
	{
		return;
	}
	
	TArray<ULockOnPointComponent*> OutComponents;
	LockOnPointComponents.Empty();
	GetOwner()->GetComponents(ULockOnPointComponent::StaticClass(), OutComponents);
	LockOnPointComponents = OutComponents;
}
