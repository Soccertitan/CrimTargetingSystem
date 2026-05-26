// Copyright Soccertitan 2026


#include "CrimTargetingSystemBlueprintFunctionLibrary.h"

#include "CrimTargetingSystemComponent.h"
#include "CrimTargetingSystemInterface.h"
#include "CrimTargetingTypes.h"
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

bool UCrimTargetingSystemBlueprintFunctionLibrary::IsVectorInView(const FMinimalViewInfo& ViewInfo, const FVector& Vector)
{
	FVector ViewDelta = Vector - ViewInfo.Location;
	ViewDelta.Normalize();
	const float ForwardDotProduct = FVector::DotProduct(ViewDelta, UKismetMathLibrary::GetForwardVector(ViewInfo.Rotation));
	
	if (ForwardDotProduct >= 0.f)
	{
		return true;
	}
	return false;
}
