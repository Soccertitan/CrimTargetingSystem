// Copyright Soccertitan 2026

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "CrimTargetingSystemBlueprintFunctionLibrary.generated.h"

class UCrimTargetingSystemComponent;
struct FCrimTargetPoint;

/**
 * 
 */
UCLASS()
class CRIMTARGETINGSYSTEM_API UCrimTargetingSystemBlueprintFunctionLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()
	
public:
	UFUNCTION(BlueprintPure, Category = "Targeting System", DisplayName = "IsValid")
	static bool IsTargetPointValid(const FCrimTargetPoint& TargetPoint);
	
	/** Will return true if the Controller can target the TargetPoint. */
	UFUNCTION(BlueprintPure, Category = "Targeting System")
	static bool IsTargetPointTargetable(AController* Controller, const FCrimTargetPoint& TargetPoint);
	
	UFUNCTION(BlueprintPure, Category = "Targeting System")
	static FVector GetTargetPointLocation(const FCrimTargetPoint& TargetPoint);
	
	/** Returns the actor that is referenced in the TargetPoint. */
	UFUNCTION(BlueprintPure, Category = "Targeting System")
	static AActor* GetActor(const FCrimTargetPoint& TargetPoint);
	
	UFUNCTION(BlueprintPure, Category = "Targeting System", DisplayName = "MakeTargetPoint")
	static FCrimTargetPoint MakeTargetPointFromHitResult(const FHitResult& HitResult);
	UFUNCTION(BlueprintPure, Category = "Targeting System", DisplayName = "MakeTargetPoint")
	static FCrimTargetPoint MakeTargetPointFromActor(AActor* Actor);
	UFUNCTION(BlueprintPure, Category = "Targeting System", DisplayName = "MakeTargetPoint")
	static FCrimTargetPoint MakeTargetPointFromSceneComponent(USceneComponent* SceneComponent, FName SocketName);
	
	UFUNCTION(BlueprintPure, Category = "Targeting System", meta = (DefaultToSelf="Actor"))
	static UCrimTargetingSystemComponent* GetCrimTargetingSystemComponent(AActor* Actor, bool bSearchComponent = true);
	
	UFUNCTION(BlueprintPure, Category = "Targeting System")
	static bool IsVectorInView(const FMinimalViewInfo& ViewInfo, const FVector& Vector);
};
