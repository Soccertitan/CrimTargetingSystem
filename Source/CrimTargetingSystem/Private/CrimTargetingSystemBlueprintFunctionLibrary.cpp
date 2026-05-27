// Copyright Soccertitan 2026


#include "CrimTargetingSystemBlueprintFunctionLibrary.h"

#include "CrimTargetingSystemComponent.h"
#include "CrimTargetingSystemInterface.h"
#include "CrimTargetingTypes.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"


bool UCrimTargetingSystemBlueprintFunctionLibrary::IsTargetPointValid(const FCrimTargetPoint& TargetPoint)
{
	return TargetPoint.IsValid();
}

bool UCrimTargetingSystemBlueprintFunctionLibrary::IsTargetPointTargetable(AController* Controller, const FCrimTargetPoint& TargetPoint)
{
	return TargetPoint.IsTargetable(Controller);
}

FVector UCrimTargetingSystemBlueprintFunctionLibrary::GetTargetPointLocation(const FCrimTargetPoint& TargetPoint)
{
	return TargetPoint.GetLocation();
}

AActor* UCrimTargetingSystemBlueprintFunctionLibrary::GetActor(const FCrimTargetPoint& TargetPoint)
{
	return TargetPoint.GetActor();
}

FCrimTargetPoint UCrimTargetingSystemBlueprintFunctionLibrary::MakeTargetPointFromHitResult(const FHitResult& HitResult)
{
	return FCrimTargetPoint(HitResult);
}

FCrimTargetPoint UCrimTargetingSystemBlueprintFunctionLibrary::MakeTargetPointFromActor(AActor* Actor)
{
	return FCrimTargetPoint(Actor);
}

FCrimTargetPoint UCrimTargetingSystemBlueprintFunctionLibrary::MakeTargetPointFromSceneComponent(USceneComponent* SceneComponent, FName SocketName)
{
	return FCrimTargetPoint(SceneComponent, SocketName);
}

UCrimTargetingSystemComponent* UCrimTargetingSystemBlueprintFunctionLibrary::GetCrimTargetingSystemComponent(AActor* Actor, bool bSearchComponent)
{
	if (Actor)
	{
		if (Actor->Implements<UCrimTargetingSystemInterface>())
		{
			return ICrimTargetingSystemInterface::Execute_GetCrimTargetingSystemComponent(Actor);
		}
		
		if (bSearchComponent)
		{
			return Actor->FindComponentByClass<UCrimTargetingSystemComponent>();
		}
	}
	return nullptr;
}

bool UCrimTargetingSystemBlueprintFunctionLibrary::IsLocationWithinView(const FMinimalViewInfo& ViewInfo, const FVector& Location)
{
	FMatrix ViewMatrix;
	FMatrix ProjectionMatrix;
    FMatrix ViewProjectionMatrix;
	UGameplayStatics::GetViewProjectionMatrix(ViewInfo, ViewMatrix, ProjectionMatrix, ViewProjectionMatrix);
	
	FVector4 ClipSpacePosition = ViewProjectionMatrix.TransformFVector4(FVector4(Location, 1.0f));
	
	if (ClipSpacePosition.W <= 0.0f)
	{
		return false; 
	}
	
	// Convert to Normalized Device Coordinates (NDC) by performing the perspective divide
	const float NDCHorizontal = ClipSpacePosition.X / ClipSpacePosition.W;
	const float NDCVertical   = ClipSpacePosition.Y / ClipSpacePosition.W;
	const float NDCDepth      = ClipSpacePosition.Z / ClipSpacePosition.W;

	// Check if the point falls cleanly inside the viewing frustum bounds
	if (FMath::IsWithinInclusive(NDCHorizontal, -1.f, 1.f) &&
		FMath::IsWithinInclusive(NDCVertical, -1.f, 1.f) &&
		FMath::IsWithinInclusive(NDCDepth, 0.f, 1.f))
	{
		return true;
	}
	
	return false;
}
