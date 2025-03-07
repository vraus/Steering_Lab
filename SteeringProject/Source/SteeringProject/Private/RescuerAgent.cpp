// Fill out your copyright notice in the Description page of Project Settings.


#include "RescuerAgent.h"

#include "Kismet/GameplayStatics.h"


ARescuerAgent::ARescuerAgent()
{
	PrimaryActorTick.bCanEverTick = true;

	TargetCharacter = nullptr;
	StartPoint = nullptr;
	EndPoint = nullptr;
	bIsFindingPath = false;
}

void ARescuerAgent::BeginPlay()
{
	Super::BeginPlay();
	
	StartPoint = FindClosestNavPoint(this);

	TargetCharacter = static_cast<ATargetCharacter*>(UGameplayStatics::GetActorOfClass(GetWorld(), ATargetCharacter::StaticClass()));

	if (TargetCharacter)
	{
		EndPoint = FindClosestNavPoint(TargetCharacter);
		// Start Path finding
	} else
	{
		UE_LOG(LogTemp, Error, TEXT("No target Found"));
	}
}

void ARescuerAgent::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	DrawDebugPath(StartPoint, EndPoint);

	if (!bIsFindingPath)
		return;
}

ANavigationPoint* ARescuerAgent::FindClosestNavPoint(const AActor* Target) const
{
	TArray<AActor*> FoundNavPoints;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), ANavigationPoint::StaticClass(), FoundNavPoints);

	ANavigationPoint* ClosestNavPoint = static_cast<ANavigationPoint*>(FoundNavPoints[0]);

	for (const auto NavPoint : FoundNavPoints)
	{
		if (ClosestNavPoint == NavPoint)
			continue;

		if (Target->GetDistanceTo(NavPoint) < Target->GetDistanceTo(ClosestNavPoint))
			ClosestNavPoint = static_cast<ANavigationPoint*>(NavPoint);
	}
	
	return ClosestNavPoint;
}

void ARescuerAgent::DrawDebugPath(const AActor* StartingPoint, const AActor* EndingPoint) const
{
	DrawDebugLine(GetWorld(), StartingPoint->GetActorLocation(), EndingPoint->GetActorLocation(), FColor::Green);
}