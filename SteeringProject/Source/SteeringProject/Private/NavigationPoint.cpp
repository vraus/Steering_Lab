// Fill out your copyright notice in the Description page of Project Settings.


#include "NavigationPoint.h"

#include "Kismet/GameplayStatics.h"


ANavigationPoint::ANavigationPoint()
{
	PrimaryActorTick.bCanEverTick = true;

	Root = CreateDefaultSubobject<USceneComponent>("Root");

	ConnectionRadius = 1200.f;
}

void ANavigationPoint::BeginPlay()
{
	Super::BeginPlay();

	FindNearbyNavPoints();
}

void ANavigationPoint::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	// DrawConnections();
}

void ANavigationPoint::FindNearbyNavPoints()
{
	TArray<AActor*> FoundActors;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), ANavigationPoint::StaticClass(), FoundActors);

	int Connected = 0;

	for (const auto Actor : FoundActors)
	{
		if (Connected == 8)
			return;

		if (CheckIfCanConnect(Actor))
		{
			ConnectedPoints.Add(static_cast<ANavigationPoint*>(Actor));
			++Connected;
		}
	}
}

bool ANavigationPoint::CheckIfCanConnect(const AActor* PotentialNeighbor) const
{
	if (!PotentialNeighbor)
		return false;

	const FVector Start = this->GetActorLocation();
	const FVector End = PotentialNeighbor->GetActorLocation();

	if (const auto Distance = FVector::Dist(Start, End); Distance > ConnectionRadius)
		return false;

	FHitResult Hit;
	FCollisionQueryParams Params;
	Params.AddIgnoredActor(this);
	Params.AddIgnoredActor(PotentialNeighbor);
	
	const auto bHit = GetWorld()->LineTraceSingleByChannel(Hit, Start, End,ECC_Visibility, Params);
	
	return !bHit;
}

void ANavigationPoint::DrawConnections() const
{
	for (const auto Connection : ConnectedPoints)
	{
		DrawDebugLine(GetWorld(), this->GetActorLocation(), Connection->GetActorLocation(), FColor::Red);
	}
}
