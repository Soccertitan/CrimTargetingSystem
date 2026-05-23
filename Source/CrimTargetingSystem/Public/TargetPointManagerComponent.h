// Copyright Soccertitan 2026

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "TargetPointManagerComponent.generated.h"


struct FInstancedStruct;
struct FGameplayTag;
class UTargetPointComponent;

/**
 * Can manage an Actor's LockOnPointComponents. Will automatically register all LockOnPoints at BeginPlay.
 */
UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class CRIMTARGETINGSYSTEM_API UTargetPointManagerComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UTargetPointManagerComponent();
	virtual void BeginPlay() override;
	
	UFUNCTION(BlueprintCallable, Category = "LockOn System")
	void SetTargetPointEnabledByTag(UPARAM(ref) const FGameplayTag& Tag, bool bEnable);
	
	UFUNCTION(BlueprintCallable, Category = "LockOn System")
	void SetTargetPointCustomDataByTag(UPARAM(ref) const FGameplayTag& Tag, const FInstancedStruct& CustomData);
	
protected:
	
private:
	UPROPERTY(Transient)
	TArray<TObjectPtr<UTargetPointComponent>> TargetPointComponents;
	
	/** Gets all LockOnPoints from the owner and adds them to the LockOnPointContainer. */
	void InitTargetPoints();
};
