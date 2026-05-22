// Copyright Soccertitan 2026

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "LockOnPointManagerComponent.generated.h"


struct FInstancedStruct;
struct FGameplayTag;
class ULockOnPointComponent;

/**
 * Can manage an Actor's LockOnPointComponents. Will automatically register all LockOnPoints at BeginPlay.
 */
UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class CRIMLOCKONSYSTEM_API ULockOnPointManagerComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	ULockOnPointManagerComponent();
	virtual void BeginPlay() override;
	
	UFUNCTION(BlueprintCallable, Category = "LockOn System")
	void SetTargetPointEnabledByTag(UPARAM(ref) const FGameplayTag& Tag, bool bEnable);
	
	UFUNCTION(BlueprintCallable, Category = "LockOn System")
	void SetLockOnPointCustomDataByTag(UPARAM(ref) const FGameplayTag& Tag, const FInstancedStruct& CustomData);
	
protected:
	
private:
	UPROPERTY(Transient)
	TArray<TObjectPtr<ULockOnPointComponent>> LockOnPointComponents;
	
	/** Gets all LockOnPoints from the owner and adds them to the LockOnPointContainer. */
	void InitLockOnPoints();
};
