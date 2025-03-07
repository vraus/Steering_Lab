// Fill out your copyright notice in the Description page of Project Settings.


#include "NavigationPoint.h"

#include "Kismet/GameplayStatics.h"


ANavigationPoint::ANavigationPoint()
{
	PrimaryActorTick.bCanEverTick = true;

	Root = CreateDefaultSubobject<USceneComponent>("Root");

	ConnectionRadius = 800.f;

	NavNode = {
		.Parent = nullptr,
		.GCost = -1,
		.HCost = -1,
		.FCost = -1,
	};
}

void ANavigationPoint::BeginPlay()
{
	Super::BeginPlay();

	FindNearbyNavPoints();
}

void ANavigationPoint::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	DrawConnections();
}

void ANavigationPoint::FindNearbyNavPoints()
{
	TArray<AActor*> FoundActors;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), ANavigationPoint::StaticClass(), FoundActors);

	for (const auto Actor : FoundActors)
	{
		if (this->GetDistanceTo(Actor) <= ConnectionRadius)
			ConnectedPoints.Add(static_cast<ANavigationPoint*>(Actor));
	}
}

void ANavigationPoint::DrawConnections() const
{
	for (const auto Connection : ConnectedPoints)
	{
		DrawDebugLine(GetWorld(), this->GetActorLocation(), Connection->GetActorLocation(), FColor::Red);
	}
}
