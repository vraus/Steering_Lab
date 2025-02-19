// Fill out your copyright notice in the Description page of Project Settings.


#include "steering_rescuer.h"

#include "MaterialHLSLTree.h"


// Sets default values
Asteering_rescuer::Asteering_rescuer()
{
	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	CachedDestinationBuffer.Add(this->GetActorLocation());
	CachedDestination = FVector::ZeroVector;
	bShouldMove = false;
}

// Called when the game starts or when spawned
void Asteering_rescuer::BeginPlay()
{
	Super::BeginPlay();
	
}

void Asteering_rescuer::MoveTo()
{
	PathOneWay();
}

void Asteering_rescuer::SetCachedDestination(const FVector& NewDestination)
{
	CachedDestinationBuffer.Add(NewDestination);
}

// Called every frame
void Asteering_rescuer::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (!bShouldMove)
		return;

	MoveTo();
}

// Called to bind functionality to input
void Asteering_rescuer::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
}

void Asteering_rescuer::MoveSeek()
{
	const FVector DesiredVelocity = (CachedDestination - this->GetActorLocation()).GetSafeNormal() * ActorInfos.MaxSpeed;
	const FVector Steering = DesiredVelocity - this->GetVelocity();

	FRotator TargetRotation = DesiredVelocity.Rotation();
	TargetRotation.Roll = 0.f;
	TargetRotation.Pitch = 0.f;

	const FRotator CurrentRotation = this->GetActorRotation();
	const FRotator SmoothedRotation = FMath::RInterpTo(CurrentRotation, TargetRotation, GetWorld()->GetDeltaSeconds(), ActorInfos.RotationSpeed);

	this->SetActorRotation(SmoothedRotation);
	this->AddMovementInput(SmoothedRotation.Vector(), Steering.Size(), true);

	DrawLine(this->GetActorLocation(), CachedDestination, FColor::Blue);
	DrawSphere(CachedDestination, 15, FColor::Red);
}

void Asteering_rescuer::MoveArrival()
{
	const FVector TargetOffset = CachedDestination - this->GetActorLocation();
	const float Distance = TargetOffset.Size();

	if  (Distance <= ActorInfos.StoppingDistance)
	{
		Velocity = FVector::ZeroVector;
		bShouldMove = false;
		return;
	}

	const float RampedSpeed = ActorInfos.MaxSpeed * (Distance / ActorInfos.StoppingDistance);
	const float ClippedSpeed = FMath::Min(RampedSpeed, ActorInfos.MaxSpeed);

	const FVector DesiredVelocity = TargetOffset.GetSafeNormal() * ClippedSpeed;

	const FVector Steering = DesiredVelocity - this->GetVelocity();

	FRotator TargetRotation = DesiredVelocity.Rotation();
	TargetRotation.Roll = 0.f;
	TargetRotation.Pitch = 0.f;

	const FRotator CurrentRotation = this->GetActorRotation();
	const FRotator SmoothedRotation = FMath::RInterpTo(CurrentRotation, TargetRotation, GetWorld()->GetDeltaSeconds(), ActorInfos.RotationSpeed);

	this->SetActorRotation(SmoothedRotation);
	this->AddMovementInput(SmoothedRotation.Vector(), Steering.Size(), true);

	DrawLine(this->GetActorLocation(), CachedDestination, FColor::Blue);
	DrawSphere(CachedDestination, ActorInfos.StoppingDistance, FColor::Red);
}

void Asteering_rescuer::PathOneWay()
{
	if (CachedDestinationBuffer.IsEmpty())
		return;

	DrawPath();

	const auto Destination = CachedDestinationBuffer[0];

	if ((this->GetActorLocation() - Destination).Size() <= ActorInfos.ValidatePathPointThreshold)
		CachedDestinationBuffer.RemoveAt(0);

	CachedDestination = Destination;

	if (CachedDestinationBuffer.Num() < 1)
		MoveArrival();
	else
		MoveSeek();

	DrawSphere(Destination, ActorInfos.ValidatePathPointThreshold, FColor::Orange);
}

void Asteering_rescuer::DrawPath() const
{
	for (int i = 0; i < CachedDestinationBuffer.Num(); i++)
		DrawSphere(CachedDestinationBuffer[i], 15, (i <= 1) ? FColor::Red : FColor::Emerald);
}

void Asteering_rescuer::DrawSphere(const FVector& Center, const float Radius, const FColor Color) const
{
	DrawDebugSphere(GetWorld(), Center, Radius, 12, Color, false, 0.f);
}

void Asteering_rescuer::DrawLine(const FVector& LineStart, const FVector& LineEnd, const FColor Color) const
{
	DrawDebugLine(GetWorld(), LineStart, LineEnd, Color, false, 0.f, 0, 2.f);
}

