// Fill out your copyright notice in the Description page of Project Settings.


#include "RescuerAgent.h"

#include "Kismet/GameplayStatics.h"


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
}

void ARescuerAgent::BeginPlay()
{
	Super::BeginPlay();
	
	StartPoint = FindClosestNavPoint(this);

	TargetCharacter = static_cast<ATargetCharacter*>(UGameplayStatics::GetActorOfClass(GetWorld(), ATargetCharacter::StaticClass()));

	if (TargetCharacter)
	{
		EndPoint = FindClosestNavPoint(TargetCharacter);
		StartPathFinding();
	} else
	{
		UE_LOG(LogTemp, Error, TEXT("No target Found"));
	}
}

void ARescuerAgent::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	DrawDebugPath(StartPoint, EndPoint);

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
	if (bIsFindingPath)
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

	if (Current == EndPoint)
	{
		bIsFindingPath = false;
		StartPathReconstruction();
		return;
	}

	OpenQueue.Remove(Current);
	ClosedQueue.Add(Current);

	// Look at every neighbor of the current point.
	for (auto Neighbor : Current->GetNeighbors())
	{
		// Ignore already explored neighbors
		if (ClosedQueue.Contains(Neighbor))
			continue;

		if (const float NewGCost = Current->NavNode.GCost + Current->GetDistanceTo(Neighbor); !OpenQueue.Contains(Neighbor) || NewGCost < Neighbor->NavNode.GCost)
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
	if (EndPoint->NavNode.Parent == nullptr)
		UE_LOG(LogTemp, Error, TEXT("No path found: EndPoint->NavNode.Parent should not be nullptr"));

	if (CurrentPathNode == StartPoint)
	{
		bIsBuildingPath = false;
		return;
	}

	Path.Add(CurrentPathNode);
	CurrentPathNode = CurrentPathNode->NavNode.Parent;		
}

void ARescuerAgent::MoveAlongPath()
{
	return;
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

void ARescuerAgent::DrawDebugPath(const AActor* StartingPoint, const AActor* EndingPoint) const
{
	DrawDebugLine(GetWorld(), StartingPoint->GetActorLocation(), EndingPoint->GetActorLocation(), FColor::Green);
}
