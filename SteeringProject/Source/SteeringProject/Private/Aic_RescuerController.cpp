// Fill out your copyright notice in the Description page of Project Settings.


#include "Aic_RescuerController.h"

AAic_RescuerController::AAic_RescuerController()
{
	bShouldMove = false;
}

void AAic_RescuerController::Tick(float DeltaTime)
{
	if (!bShouldMove || Agent == nullptr)
		return;

	MoveTo();
}

void AAic_RescuerController::AddDestinationToCache(FVector NewDestination)
{
	CachedDestinationBuffer.Add(NewDestination);

	bShouldMove = true;
}

void AAic_RescuerController::MoveTo()
{
	if (CachedDestinationBuffer.IsEmpty()) {
		bShouldMove = false;
		return;
	}

	DrawPath();

	const auto Destination = CachedDestinationBuffer[0];

	if ((Agent->GetActorLocation() - Destination).Size() <= Agent->ActorInfos.ValidatePathPointThreshold)
		CachedDestinationBuffer.RemoveAt(0);

	CachedDestination = Destination;

	MoveSeek();

	DrawSphere(Destination, Agent->ActorInfos.ValidatePathPointThreshold, FColor::Orange);
}

void AAic_RescuerController::MoveSeek()
{
	const FVector DesiredVelocity = (CachedDestination - Agent->GetActorLocation()).GetSafeNormal() * Agent->ActorInfos.MaxSpeed;

	const FVector Steering = DesiredVelocity - Agent->GetVelocity();

	FRotator TargetRotation = DesiredVelocity.Rotation();
	TargetRotation.Roll = 0.f;
	TargetRotation.Pitch = 0.f;

	const FRotator CurrentRotation = Agent->GetActorRotation();
	const FRotator SmoothedRotation = FMath::RInterpTo(CurrentRotation, TargetRotation, GetWorld()->GetDeltaSeconds(), Agent->ActorInfos.RotationSpeed);

	Agent->SetActorRotation(SmoothedRotation);

	Agent->AddMovementInput(SmoothedRotation.Vector(), Steering.Size(), true);

	DrawLine(Agent->GetActorLocation(), CachedDestination, FColor::Blue);
	DrawSphere(CachedDestination, 15, FColor::Red);
}

void AAic_RescuerController::DrawPath() const
{
	for (int i = 0; i < CachedDestinationBuffer.Num(); i++)
		DrawSphere(CachedDestinationBuffer[i], 15, (i <= 1) ? FColor::Red : FColor::Emerald);
}

void AAic_RescuerController::DrawSphere(const FVector& Center, const float Radius, const FColor Color) const
{
	DrawDebugSphere(GetWorld(), Center, Radius, 12, Color, false, 0.f);
}

void AAic_RescuerController::DrawLine(const FVector& LineStart, const FVector& LineEnd, const FColor Color) const
{
	DrawDebugLine(GetWorld(), LineStart, LineEnd, Color, false, 0.f, 0, 2.f);
}
