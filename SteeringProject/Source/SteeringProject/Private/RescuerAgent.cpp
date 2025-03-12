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

	bTargetIsCharacter = true;
	State = ERescuerStates::Wander;

	Velocity = FVector::ZeroVector;
	CachedLocation = FVector::ZeroVector;
	
	Player_Stats.Mass = 1.f;
	Player_Stats.MaxSpeed = 600.f;
	Player_Stats.MaxForce = 600.f;
	Player_Stats.RotationSpeed = 15.f;
	Player_Stats.SlowingDistance = 600.f;
	Player_Stats.StoppingDistance = 150.f;
	Player_Stats.FleeThreshold = 90.f;
	Player_Stats.EvadeCooldown = .25f;
	Player_Stats.EvadeCooldown = 2.f;
	Player_Stats.ValidatePathPointThreshold = 200.f;	
}

void ARescuerAgent::InitTarget(AActor* Target, const bool bIsCharacter)
{
	ClearQueues();

	bTargetIsCharacter = bIsCharacter;
	
	TargetCharacter = nullptr;
	StartPoint = nullptr;
	EndPoint = nullptr;
	CurrentPathNode = nullptr;
	Velocity = FVector::ZeroVector;
	CachedLocation = FVector::ZeroVector;
	
	TargetCharacter = Target;

	if (TargetCharacter == nullptr)
		UE_LOG(LogTemp, Error, TEXT("Target is null"));

	Ready();
}

void ARescuerAgent::Ready()
{
	StartPoint = FindClosestNavPoint(this);

	if (!StartPoint)
		UE_LOG(LogTemp, Error, TEXT("StartPoint is null"));

	if (TargetCharacter)
	{
		EndPoint = FindClosestNavPoint(TargetCharacter);
		StartPathFinding();
	}
	else
		UE_LOG(LogTemp, Error, TEXT("Target is null"));
}

void ARescuerAgent::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	
	DebugClosedQueue();

	switch (State) 
	{
	case ERescuerStates::Wander:
		break;
	case ERescuerStates::SearchPath:
		if (OpenQueue.Num() > 0)
			FindPath();
		break;
	case ERescuerStates::BuildPath:
		ReconstructPath();
		break;
	case ERescuerStates::Move:
		MoveAlongPath();
		break;
	}
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

	FSNavNode& StartNode = NavData.FindOrAdd(StartPoint);
	StartNode.GCost = 0;
	StartNode.HCost = StartPoint->GetDistanceTo(TargetCharacter);
	StartNode.Parent = nullptr;
	
	OpenQueue.Add(StartPoint);
	
	State = ERescuerStates::SearchPath;
}

void ARescuerAgent::FindPath()
{
	ANavigationPoint* Current = GetLowestFCost();
	OpenQueue.Remove(Current);
	ClosedQueue.Add(Current);

	FSNavNode& CurrentNode = NavData.FindOrAdd(Current);

	// Look at every neighbor of the current point.
	for (auto Neighbor : Current->GetNeighbors())
	{
		// Ignore already explored neighbors
		if (ClosedQueue.Contains(Neighbor))
			continue;

		FSNavNode& NeighborNode = NavData.FindOrAdd(Neighbor);
		
		const float NewGCost = CurrentNode.GCost + Current->GetDistanceTo(Neighbor);
		
		if (Neighbor == EndPoint)
		{
			NeighborNode.Parent = Current;
			StartPathReconstruction();
			return;
		}
		
		if ( !OpenQueue.Contains(Neighbor) || NewGCost < NeighborNode.GCost || NeighborNode.GCost == -1)
		{
			NeighborNode.Parent = Current;
			NeighborNode.GCost = NewGCost;

			if (!OpenQueue.Contains(Neighbor))
			{
				NeighborNode.HCost = Neighbor->GetDistanceTo(EndPoint);
				OpenQueue.Add(Neighbor);
			}
		}
	}
}

void ARescuerAgent::StartPathReconstruction()
{
	CurrentPathNode = EndPoint;
	State = ERescuerStates::BuildPath;
}

void ARescuerAgent::ReconstructPath()
{
	if (!CurrentPathNode)
	{
		UE_LOG(LogTemp, Error, TEXT("PATH FOUND !"));
		State = ERescuerStates::Move;
		return;
	}

	Path.Add(CurrentPathNode);
	CurrentPathNode = NavData[CurrentPathNode].Parent;
}

FVector ARescuerAgent::DetectAgentInFront() const
{
	FVector AvoidanceOffset = FVector::ZeroVector;

	const FVector Start = GetActorLocation();
	const FVector Forward = GetActorForwardVector();
	const FVector End = Start + (Forward * 300.0f);

	FHitResult Hit;
	FCollisionQueryParams Params;
	Params.AddIgnoredActor(this);

	if (GetWorld()->LineTraceSingleByChannel(Hit, Start, End, ECC_Pawn, Params))
	{
		if (Hit.GetActor() && Hit.GetActor()->IsA(AActor::StaticClass()))
		{
			const FVector RightVector = GetActorRightVector();
			AvoidanceOffset = RightVector * 100.0f;
		}
	}
	
	return AvoidanceOffset;
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
		State = ERescuerStates::Wander;
		OnTargetReached.Broadcast(this, bTargetIsCharacter);
		return;
	}

	const auto Destination = Path.Last()->GetActorLocation();

	if ((this->GetActorLocation() - Destination).Size() <= Player_Stats.ValidatePathPointThreshold)
		Path.RemoveAt(Path.Num() - 1);

	CachedLocation = Destination;

	if (Path.Num() == 0)
		MoveArrival();
	else 
		MoveSeek();
}

void ARescuerAgent::MoveSeek()
{
	const FVector DesiredVelocity = ((CachedLocation - this->GetActorLocation()).GetSafeNormal() * Player_Stats.MaxSpeed) + DetectAgentInFront();

	const FVector Steering = DesiredVelocity - this->GetVelocity();

	FRotator TargetRotation = DesiredVelocity.Rotation();
	TargetRotation.Roll = 0.f;
	TargetRotation.Pitch = 0.f;

	const FRotator CurrentRotation = this->GetActorRotation();
	const FRotator SmoothedRotation = FMath::RInterpTo(CurrentRotation, TargetRotation, GetWorld()->GetDeltaSeconds(), Player_Stats.RotationSpeed);

	this->SetActorRotation(SmoothedRotation);

	this->AddMovementInput(SmoothedRotation.Vector(), Steering.Size(), true);
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

	const FVector DesiredVelocity = (TargetOffset.GetSafeNormal() * ClippedSpeed) + DetectAgentInFront();

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
		if (NavData[Node].FCost() < NavData[LowestFCost].FCost())
			LowestFCost = Node;
	}

	return LowestFCost;
}

void ARescuerAgent::ClearQueues()
{
	OpenQueue.Empty();
	ClosedQueue.Empty();
	Path.Empty();
	NavData.Empty();
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

void ARescuerAgent::DebugClosedQueue() const
{
	for (const auto Node : ClosedQueue)
	{
		DrawSphere(Node->GetActorLocation(), 50, FColor::Purple);
	}
}

void ARescuerAgent::DebugOpenQueue() const
{
	for (const auto Node : OpenQueue)
	{
		DrawSphere(Node->GetActorLocation(), 50, FColor::White);
	}
}
