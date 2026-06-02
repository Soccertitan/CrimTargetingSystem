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
	
	/** 
	 * Gets the 2d position relative to the view projection matrix. Returns false if the point is out of bounds the matrix.
	 * @param ViewProjectionMatrix The projection matrix to check the point against.
	 * @param WorldPoint The 3d world position to check.
	 * @param OutPosition The scaled position relative to the center of the view projection.
	 */
	UFUNCTION(BlueprintPure, Category = "Targeting System")
	static bool ProjectWorldPointToViewProjectionMatrix(const FMatrix& ViewProjectionMatrix, const FVector& WorldPoint, FVector2D& OutPosition);
};
