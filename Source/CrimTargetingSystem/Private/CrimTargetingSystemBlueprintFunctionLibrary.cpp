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
