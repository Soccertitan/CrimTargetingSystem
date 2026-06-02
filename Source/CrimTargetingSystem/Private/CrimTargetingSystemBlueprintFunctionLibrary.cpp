// Copyright Soccertitan 2026


#include "CrimTargetingSystemBlueprintFunctionLibrary.h"

#include "CrimTargetingSystemComponent.h"
#include "CrimTargetingSystemInterface.h"
#include "CrimTargetingTypes.h"


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

bool UCrimTargetingSystemBlueprintFunctionLibrary::ProjectWorldPointToViewProjectionMatrix(const FMatrix& ViewProjectionMatrix, const FVector& WorldPoint, FVector2D& OutPosition)
{
	FPlane Result = ViewProjectionMatrix.TransformFVector4(FVector4(WorldPoint, 1.f));
	
	// If W is less than or equal to 0, the WorldPoint is behind the projection.
	const bool bIsInsideView = Result.W > 0.f;
	if (!bIsInsideView)
	{
		return false;
	}
	
	// Convert to Normalized Device Coordinates (NDC) by performing the perspective divide
	const float NDCHorizontal = Result.X / Result.W;
	const float NDCVertical   = Result.Y / Result.W;
	const float NDCDepth      = Result.Z / Result.W;
	
	// Check if the WorldPoint falls cleanly inside the viewing frustum bounds
	if (!FMath::IsWithinInclusive(NDCHorizontal, -1.f, 1.f) ||
		!FMath::IsWithinInclusive(NDCVertical, -1.f, 1.f) ||
		!FMath::IsWithinInclusive(NDCDepth, 0.f, 1.f))
	{
		return false;
	}
	
	// Scale the OutPosition to the view.
	float XScale = FVector(ViewProjectionMatrix.M[0][1], ViewProjectionMatrix.M[1][1], ViewProjectionMatrix.M[2][1]).Size();
	float YScale = FVector(ViewProjectionMatrix.M[0][0], ViewProjectionMatrix.M[1][0], ViewProjectionMatrix.M[2][0]).Size();
	
	const float RHW = 1.f / Result.W;
	FPlane PosInScreenSpace = FPlane(Result.X * RHW, Result.Y * RHW, Result.Z * RHW, Result.W);
	// Multiply by 1000 to have a position that is greater than a single unit. Not sure if this is necessary but, it makes 
	// it easier for me to compare larger whole numbers while debugging.
	OutPosition.X = PosInScreenSpace.X * XScale * 1000;
	OutPosition.Y = PosInScreenSpace.Y * YScale * 1000;
	
	return bIsInsideView;
}
