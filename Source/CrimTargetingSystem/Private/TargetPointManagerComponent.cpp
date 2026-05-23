// Copyright Soccertitan 2026


#include "TargetPointManagerComponent.h"

#include "TargetPointComponent.h"


UTargetPointManagerComponent::UTargetPointManagerComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
	SetIsReplicatedByDefault(false);
}

void UTargetPointManagerComponent::BeginPlay()
{
	Super::BeginPlay();

	InitTargetPoints();	
}

void UTargetPointManagerComponent::SetTargetPointEnabledByTag(const FGameplayTag& Tag, bool bEnable)
{
	for (TObjectPtr<UTargetPointComponent>& Component : TargetPointComponents)
	{
		if (Component && Component->GetLockOnPointTag().MatchesTag(Tag))
		{
			Component->SetIsTargetable(bEnable);
		}
	}
}

void UTargetPointManagerComponent::SetTargetPointCustomDataByTag(const FGameplayTag& Tag, const FInstancedStruct& CustomData)
{
	if (CustomData.IsValid())
	{
		for (TObjectPtr<UTargetPointComponent>& Component : TargetPointComponents)
		{
			if (Component && Component->GetLockOnPointTag().MatchesTag(Tag))
			{
				Component->SetCustomData(CustomData);
			}
		}
	}
}

void UTargetPointManagerComponent::InitTargetPoints()
{
	if (IsNetSimulating())
	{
		return;
	}
	
	TArray<UTargetPointComponent*> OutComponents;
	TargetPointComponents.Empty();
	GetOwner()->GetComponents(UTargetPointComponent::StaticClass(), OutComponents);
	TargetPointComponents = OutComponents;
}
