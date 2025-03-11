// Fill out your copyright notice in the Description page of Project Settings.


#include "RescuerAgent.h"

#include "Kismet/GameplayStatics.h"
#include "WorldPartition/ContentBundle/ContentBundleLog.h"


ARescuerAgent::ARescuerAgent()
{
	PrimaryActorTick.bCanEverTick = true;

	TargetCharacter = nullptr;
	StartPoint = nullptr;
	EndPoint = nullptr;
	CurrentPathNode = nullptr;
	
	bIsFindingPath = false;
	bIsBuildingPath = false;
	bCanMove = false;

	Velocity = FVector::ZeroVector;
	CachedLocation = FVector::ZeroVector;
	
	Player_Stats.Mass = 1.f;
	Player_Stats.MaxSpeed = 600.f;
	Player_Stats.MaxForce = 600.f;
	Player_Stats.RotationSpeed = 6.f;
	Player_Stats.SlowingDistance = 600.f;
	Player_Stats.StoppingDistance = 50.f;
	Player_Stats.FleeThreshold = 90.f;
	Player_Stats.EvadeCooldown = .25f;
	Player_Stats.EvadeCooldown = 2.f;
	Player_Stats.ValidatePathPointThreshold = 150.f;	
}

void ARescuerAgent::InitTarget(ATargetCharacter* Target)
{
	TargetCharacter = Target;

	if (TargetCharacter == nullptr)
		UE_LOG(LogTemp, Error, TEXT("Target is null"));

	Ready();
}

void ARescuerAgent::Ready()
{
	StartPoint = FindClosestNavPoint(this);

	if (TargetCharacter)
	{
		EndPoint = FindClosestNavPoint(TargetCharacter);
		StartPathFinding();
	}
}

void ARescuerAgent::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);


	// We only start to find path once we're all set up
	if (bIsFindingPath)
	{
		// When Open Queue is empty we either have a path or the target is not reachable.
		if (OpenQueue.Num() <= 0)
			return;

		FindPath();
	}

	// Build the path
	if (bIsBuildingPath)
		ReconstructPath();

	// The character knows the best route and can now move along the path to the destination
	if (bCanMove)
		MoveAlongPath();

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

void ARescuerAgent::StartPathFinding()
{
	ClearQueues();

	StartPoint->NavNode.GCost = 0;
	StartPoint->NavNode.HCost = StartPoint->GetDistanceTo(TargetCharacter);
	OpenQueue.Add(StartPoint);
	
	bIsFindingPath = true;
}

void ARescuerAgent::FindPath()
{
	ANavigationPoint* Current = GetLowestFCost();
	OpenQueue.Remove(Current);
	ClosedQueue.Add(Current);

	// Look at every neighbor of the current point.
	for (auto Neighbor : Current->GetNeighbors())
	{
		// Ignore already explored neighbors
		if (ClosedQueue.Contains(Neighbor))
			continue;
		
		if (Neighbor == EndPoint)
		{
			bIsFindingPath = false;
			EndPoint->NavNode.Parent = Current;
			StartPathReconstruction();
			return;
		}

		if (const float NewGCost = Current->NavNode.GCost + Current->GetDistanceTo(Neighbor); !OpenQueue.Contains(Neighbor) || NewGCost < Neighbor->NavNode.GCost || Neighbor->NavNode.GCost == -1)
		{
			Neighbor->NavNode.Parent = Current;
			Neighbor->NavNode.GCost = NewGCost;

			if (!OpenQueue.Contains(Neighbor))
			{
				Neighbor->NavNode.HCost = Neighbor->GetDistanceTo(EndPoint);
				OpenQueue.Add(Neighbor);
			}
		}
	}
}

void ARescuerAgent::StartPathReconstruction()
{
	CurrentPathNode = EndPoint;
	bIsBuildingPath = true;
}

void ARescuerAgent::ReconstructPath()
{
	if (CurrentPathNode == nullptr)
	{
		bIsBuildingPath = false;
		bCanMove = true;
		return;
	}

	Path.Add(CurrentPathNode);
	CurrentPathNode = CurrentPathNode->NavNode.Parent;
}

void ARescuerAgent::MoveAlongPath()
{
	DrawDebugPath();
	PathOneWay();
}

void ARescuerAgent::PathOneWay()
{
	if (Path.IsEmpty())
	{
		bCanMove = false;	
		return;
	}

	const auto Destination = Path.Last()->GetActorLocation();

	if ((this->GetActorLocation() - Destination).Size() <= Player_Stats.ValidatePathPointThreshold)
		Path.RemoveAt(Path.Num() - 1);

	CachedLocation = Destination;

	MoveArrival();
}

void ARescuerAgent::MoveArrival()
{
	const FVector TargetOffset = CachedLocation - this->GetActorLocation();
	const float Distance = TargetOffset.Size();

	if (Distance <= Player_Stats.StoppingDistance)
	{
		Velocity = FVector::ZeroVector;
		return;
	}

	const float RampedSpeed = Player_Stats.MaxSpeed * (Distance / Player_Stats.SlowingDistance);
	const float ClippedSpeed = FMath::Min(RampedSpeed, Player_Stats.MaxSpeed);

	const FVector DesiredVelocity = TargetOffset.GetSafeNormal() * ClippedSpeed;

	const FVector Steering = DesiredVelocity - this->GetVelocity();

	FRotator TargetRotation = DesiredVelocity.Rotation();
	TargetRotation.Roll = 0.f;
	TargetRotation.Pitch = 0.f;

	const FRotator CurrentRotation = this->GetActorRotation();
	const FRotator SmoothedRotation = FMath::RInterpTo(CurrentRotation, TargetRotation, GetWorld()->GetDeltaSeconds(), Player_Stats.RotationSpeed);

	this->SetActorRotation(SmoothedRotation);

	this->AddMovementInput(SmoothedRotation.Vector(), Steering.Size(), true);
	
	DrawSphere(CachedLocation, Player_Stats.StoppingDistance, FColor::Orange);
}

ANavigationPoint* ARescuerAgent::GetLowestFCost() const
{
	ANavigationPoint* LowestFCost = OpenQueue[0];

	for (const auto Node : OpenQueue)
	{
		if (Node->FCost() < LowestFCost->FCost())
			LowestFCost = Node;
	}

	return LowestFCost;
}

void ARescuerAgent::ClearQueues()
{
	OpenQueue.Empty();
	ClosedQueue.Empty();
	Path.Empty();
}

void ARescuerAgent::DrawDebugPath() const
{
	for (int i = 0; i < Path.Num()-1; ++i)
	{
		DrawLine(Path[i], Path[i+1], FColor::Green);
	}
}

void ARescuerAgent::DrawLine(const AActor* StartingPoint, const AActor* EndingPoint, const FColor Color) const
{
	DrawDebugLine(GetWorld(), StartingPoint->GetActorLocation(), EndingPoint->GetActorLocation(), Color);
}

void ARescuerAgent::DrawSphere(const FVector& Center, const float Radius, const FColor Color) const
{
	DrawDebugSphere(GetWorld(), Center, Radius, 12, Color, false, 0.f);
}
