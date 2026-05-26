// Copyright Soccertitan 2026

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "TargetPointInterface.h"
#include "Components/SphereComponent.h"
#include "StructUtils/InstancedStruct.h"
#include "TargetPointComponent.generated.h"


/**
 * A simple sphere component that can be locked onto. Can be managed by a TargetPointManagerComponent.
 */
UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class CRIMTARGETINGSYSTEM_API UTargetPointComponent : public USphereComponent, public ITargetPointInterface
{
	GENERATED_BODY()

public:
	UTargetPointComponent();
	virtual void GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const override;
	virtual void PreNetReceive() override;
	virtual void OnRegister() override;
	virtual void BeginPlay() override;
	
	UFUNCTION(BlueprintPure, Category = "LockOn System")
	FGameplayTag GetLockOnPointTag() const { return LockOnPointTag; }
	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly, Category = "LockOn System")
	void SetLockOnPointTag(UPARAM(ref) FGameplayTag& InLockOnPointTag);
	
	/** ILockOnPointInterface */
	virtual bool IsTargetPointTargetable_Implementation(AActor* Actor) override;
	
	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly, Category = "LockOn System")
	void SetIsTargetable(const bool bEnable);
	
	UFUNCTION(BlueprintPure, Category = "LockOn System")
	const FInstancedStruct& GetCustomData() const { return CustomData; }
	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly, Category = "LockOn System")
	void SetCustomData(const FInstancedStruct& InCustomData);
	
protected:
	UFUNCTION()
	void OnRep_CustomData();
	
	virtual void OnCustomDataSet(){}
	
	bool HasAuthority() const;
	
private:
	UPROPERTY(EditDefaultsOnly, Replicated)
	FGameplayTag LockOnPointTag;
	
	UPROPERTY(EditDefaultsOnly, Replicated)
	bool bTargetable = true;
	
	UPROPERTY(EditDefaultsOnly, ReplicatedUsing = "OnRep_CustomData")
	FInstancedStruct CustomData;
	
	/** Cached value of whether our owner is a simulated Actor. */
	bool bCachedIsNetSimulated = false;
	void CacheIsNetSimulated();
};
