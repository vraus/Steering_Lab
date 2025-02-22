// Fill out your copyright notice in the Description page of Project Settings.

#include "steering_player_controller.h"
#include "Kismet/KismetMathLibrary.h"
#include "GameFramework/Pawn.h"
#include "steering_character.h"
#include "Engine/World.h"
#include "EnhancedInputComponent.h"
#include "InputActionValue.h"
#include "EnhancedInputSubsystems.h"
#include "Engine/LocalPlayer.h"

Asteering_player_controller::Asteering_player_controller(): ShortPressThreshold(0), DefaultMappingContext(nullptr),
                                                            SetDestinationClickAction(nullptr),
                                                            bMoveToMouseCursor(false)
{
	TargetCharacter = nullptr;
	character_ = nullptr;
	Player_Stats = {};
	
	bShowMouseCursor = true;
	DefaultMouseCursor = EMouseCursor::Default;
	CachedDestination = FVector::ZeroVector;
	CachedDestinationBuffer.Add(FVector::ZeroVector);
	FollowTime = 0.f;
	Behaviour = EBehaviours::Undefined;

	LastEvadeTime = 0.f;
	bPathMode = false;
	bShouldMove = false;
	Velocity = FVector::ZeroVector;

	TargetCharacter_Location = FVector::ZeroVector;
	TargetCharacter_Velocity = FVector::ZeroVector;
}

void Asteering_player_controller::SetMovementBehaviour(const EBehaviours New_Behaviour)
{
	Behaviour = New_Behaviour;

	// Unsure that the character start chasing or evading when New behavior set to said behaviors, this even if bShouldMove is false meaning the player is not curently moving.
	switch (Behaviour)
	{
	case EBehaviours::Arrival:
		bPathMode = false;
		break;
	case EBehaviours::Seek:
		bPathMode = false;
		break;
	case EBehaviours::Flee:
		bPathMode = false;
    	break;
	case EBehaviours::Pursuit:
		bShouldMove = true;
		bPathMode = false;
		break;
	case EBehaviours::Evade:
		bShouldMove = true;
		bPathMode = false;
		break;
	case EBehaviours::Circuit:
		ResetCachedDestinationBuffer();
		bShouldMove = false;
		bPathMode = true;
		break;
	case EBehaviours::OneWay:
		ResetCachedDestinationBuffer();
		bShouldMove = false;
		bPathMode = true;
		break;
	case EBehaviours::TwoWay:
		ResetCachedDestinationBuffer();
		bShouldMove = false;
		bPathMode = true;
		break;
	default:
			break;
	}
	
	UE_LOG(LogTemp, Display, TEXT("New Behaviour received"));
}

void Asteering_player_controller::SetCharacter(Asteering_character* Player_Pawn)
{
	if (Player_Pawn) {
		character_ = Player_Pawn;
		Player_Stats = character_->GetActorInfos();
	}
	else {
		UE_LOG(LogTemp, Error, TEXT("Error: invalid player_pawn."));
	}
}

void Asteering_player_controller::SetTargetCharacter(ATargetCharacter* Target_Character)
{
	if (Target_Character)
	{
		TargetCharacter = Target_Character;
	}
}

void Asteering_player_controller::BeginPlay()
{
	Super::BeginPlay();
}

void Asteering_player_controller::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	if (!bShouldMove) return;

	if (TargetCharacter != nullptr) {
		TargetCharacter_Location = TargetCharacter->GetActorLocation();
		TargetCharacter_Velocity = TargetCharacter->GetVelocity();
	}

	MoveTo();
}

void Asteering_player_controller::OnInputStarted()
{
	StopMovement();

	if (!bPathMode)
		bShouldMove = true;
}

void Asteering_player_controller::OnGoPath()
{
	CachedDestinationBuffer[0] = character_->GetActorLocation();

	if (int i = CachedDestinationBuffer.Num(); Behaviour == EBehaviours::TwoWay && i > 1)
	{
		for (i = i - 2; i > 0; --i)
		{
			auto Tmp = CachedDestinationBuffer[i];
			CachedDestinationBuffer.Add(Tmp);	
		}
	}
	
	bPathMode = true;
	bShouldMove = true;
}

void Asteering_player_controller::MoveTo()
{
	switch (Behaviour)
	{
	case EBehaviours::Undefined:
		UE_LOG(LogTemp, Error, TEXT("Undefined"));
		break;
	case EBehaviours::Seek:
		MoveSeek();
		break;
	case EBehaviours::Flee:
		MoveFlee();
		break;
	case EBehaviours::Pursuit:
		MovePursuit();
		break;
	case EBehaviours::Evade:
		MoveEvade();
		break;
	case EBehaviours::Arrival:
		MoveArrival();
		break;
	case EBehaviours::Circuit:
		PathCircuit();
		break;
	case EBehaviours::OneWay:
		PathOneWay();
		break;
	case EBehaviours::TwoWay:
		PathTwoWay();
		break;
	default:
		UE_LOG(LogTemp, Log, TEXT("Default"));
		break;
	}
}

void Asteering_player_controller::SetCachedLocation(const FVector& NewLocation)
{
	if (!bPathMode)
	{
		CachedDestination = NewLocation;
		return;
	}
	
	CachedDestinationBuffer.Add(NewLocation);
}

void Asteering_player_controller::MoveSeek() const
{
	const FVector DesiredVelocity = (CachedDestination - character_->GetActorLocation()).GetSafeNormal() * Player_Stats.MaxSpeed;

	const FVector Steering = DesiredVelocity - character_->GetVelocity();

	FRotator TargetRotation = DesiredVelocity.Rotation();
	TargetRotation.Roll = 0.f;
	TargetRotation.Pitch = 0.f;

	const FRotator CurrentRotation = character_->GetActorRotation();
	const FRotator SmoothedRotation = FMath::RInterpTo(CurrentRotation, TargetRotation, GetWorld()->GetDeltaSeconds(), Player_Stats.RotationSpeed);

	character_->SetActorRotation(SmoothedRotation);

	character_->AddMovementInput(SmoothedRotation.Vector(), Steering.Size(), true);

	DrawLine(character_->GetActorLocation(), CachedDestination, FColor::Blue);
	DrawSphere(CachedDestination, 15, FColor::Red);
}

void Asteering_player_controller::MoveFlee() const
{
	const FVector DesiredVelocity = (character_->GetActorLocation() - CachedDestination).GetSafeNormal() * Player_Stats.MaxSpeed;

	const FVector Steering = DesiredVelocity - character_->GetVelocity();

	FRotator TargetRotation = DesiredVelocity.Rotation();
	TargetRotation.Roll = 0.f;
	TargetRotation.Pitch = 0.f;

	const FRotator CurrentRotation = character_->GetActorRotation();
	const FRotator SmoothedRotation = FMath::RInterpTo(CurrentRotation, TargetRotation, GetWorld()->GetDeltaSeconds(), Player_Stats.RotationSpeed);

	character_->SetActorRotation(SmoothedRotation);

	character_->AddMovementInput(SmoothedRotation.Vector(), Steering.Size(), true);

	DrawLine(character_->GetActorLocation(), CachedDestination, FColor::Blue);
	DrawSphere(CachedDestination, 15, FColor::Red);
}

void Asteering_player_controller::MovePursuit() const
{
	const FVector Target_FuturLocation = TargetCharacter_Location + TargetCharacter_Velocity * Player_Stats.PursuitPrediction;

	const FVector DesiredVelocity = (Target_FuturLocation - character_->GetActorLocation()).GetSafeNormal() * Player_Stats.MaxSpeed;
	const FVector Steering = DesiredVelocity - character_->GetVelocity();

	FRotator TargetRotation = DesiredVelocity.Rotation();
	TargetRotation.Roll = 0.f;
	TargetRotation.Pitch = 0.f;

	const FRotator CurrentRotation = character_->GetActorRotation();
	const FRotator SmoothedRotation = FMath::RInterpTo(CurrentRotation, TargetRotation, GetWorld()->GetDeltaSeconds(), Player_Stats.RotationSpeed);

	character_->SetActorRotation(SmoothedRotation);

	character_->AddMovementInput(SmoothedRotation.Vector(), Steering.Size(), true);

	DrawLine(character_->GetActorLocation(), Target_FuturLocation, FColor::Blue);
	DrawSphere(Target_FuturLocation, 15, FColor::Red);
}

void Asteering_player_controller::MoveEvade()
{
    const FVector Target_FutureLocation = TargetCharacter_Location + TargetCharacter_Velocity * Player_Stats.PursuitPrediction;

    const float Distance = (Target_FutureLocation - character_->GetActorLocation()).Size();
    FVector EvadeDirection = (character_->GetActorLocation() - Target_FutureLocation).GetSafeNormal();
	const float CurrentTime = GetWorld()->GetTimeSeconds();
	
    if (Distance <= Player_Stats.FleeThreshold && CurrentTime - LastEvadeTime > Player_Stats.EvadeCooldown)
    {
    	const float RandomAngle = (FMath::RandBool()? 90.0f : -90.0f);
    	EvadeDirection = character_->GetActorForwardVector().RotateAngleAxis(RandomAngle, FVector::UpVector);
    	LastEvadeTime = CurrentTime;
    }

    const FVector DesiredVelocity = EvadeDirection * Player_Stats.MaxSpeed;
    const FVector Steering = DesiredVelocity - character_->GetVelocity();

    FRotator TargetRotation = DesiredVelocity.Rotation();
    TargetRotation.Roll = 0.f;
    TargetRotation.Pitch = 0.f;

    const FRotator CurrentRotation = character_->GetActorRotation();
    const FRotator SmoothedRotation = FMath::RInterpTo(CurrentRotation, TargetRotation, GetWorld()->GetDeltaSeconds(), 
                                                 (Distance <= Player_Stats.FleeThreshold) ? Player_Stats.RotationSpeed * 2 : Player_Stats.RotationSpeed);

    character_->SetActorRotation(SmoothedRotation);

    character_->AddMovementInput(character_->GetActorForwardVector(), Steering.Size(), true);

	
	DrawLine(character_->GetActorLocation(), Target_FutureLocation, FColor::Blue);
	DrawSphere(Target_FutureLocation, 15, FColor::Red);

	DrawSphere(character_->GetActorLocation(), Player_Stats.FleeThreshold, FColor::Green);
}


void Asteering_player_controller::MoveArrival()
{
	const FVector TargetOffset = CachedDestination - character_->GetActorLocation();
	const float Distance = TargetOffset.Size();

	if (Distance <= Player_Stats.StoppingDistance)
	{
		Velocity = FVector::ZeroVector;
		bShouldMove = false;
		return;
	}

	const float RampedSpeed = Player_Stats.MaxSpeed * (Distance / Player_Stats.SlowingDistance);
	const float ClippedSpeed = FMath::Min(RampedSpeed, Player_Stats.MaxSpeed);

	const FVector DesiredVelocity = TargetOffset.GetSafeNormal() * ClippedSpeed;

	const FVector Steering = DesiredVelocity - character_->GetVelocity();

	FRotator TargetRotation = DesiredVelocity.Rotation();
	TargetRotation.Roll = 0.f;
	TargetRotation.Pitch = 0.f;

	const FRotator CurrentRotation = character_->GetActorRotation();
	const FRotator SmoothedRotation = FMath::RInterpTo(CurrentRotation, TargetRotation, GetWorld()->GetDeltaSeconds(), Player_Stats.RotationSpeed);

	character_->SetActorRotation(SmoothedRotation);

	character_->AddMovementInput(SmoothedRotation.Vector(), Steering.Size(), true);

	DrawLine(character_->GetActorLocation(), CachedDestination, FColor::Blue);
	DrawSphere(CachedDestination, Player_Stats.StoppingDistance, FColor::Red);
}

void Asteering_player_controller::PathCircuit()
{
	if (CachedDestinationBuffer.IsEmpty())
		return;

	DrawPath();
	
	const auto Destination = CachedDestinationBuffer[0];

	if ((character_->GetActorLocation() - Destination).Size() <= Player_Stats.ValidatePathPointThreshold)
	{
		CachedDestinationBuffer.RemoveAt(0);
		CachedDestinationBuffer.Add(Destination);
	}
	
	CachedDestination = Destination;

	MoveSeek();

	DrawSphere(Destination, Player_Stats.ValidatePathPointThreshold, FColor::Orange);
}

void Asteering_player_controller::PathOneWay()
{
	if (CachedDestinationBuffer.IsEmpty())
		return;
	
	DrawPath();
	
	const auto Destination = CachedDestinationBuffer[0];

	if ((character_->GetActorLocation() - Destination).Size() <= Player_Stats.ValidatePathPointThreshold)
		CachedDestinationBuffer.RemoveAt(0);

	CachedDestination = Destination;

	if (CachedDestinationBuffer.Num() < 1)
	{
		MoveArrival();
	}
	else
	{
		MoveSeek();
	}

	DrawSphere(Destination, Player_Stats.ValidatePathPointThreshold, FColor::Orange);
}

void Asteering_player_controller::PathTwoWay()
{
	if (CachedDestinationBuffer.IsEmpty())
		return;

	DrawPath();
	
	const auto Destination = CachedDestinationBuffer[0];

	if ((character_->GetActorLocation() - Destination).Size() <= Player_Stats.ValidatePathPointThreshold)
	{
		CachedDestinationBuffer.RemoveAt(0);
		CachedDestinationBuffer.Add(Destination);
	}
	
	CachedDestination = Destination;

	MoveSeek();

	DrawSphere(Destination, Player_Stats.ValidatePathPointThreshold, FColor::Orange);
}

void Asteering_player_controller::ResetCachedDestinationBuffer()
{
	CachedDestinationBuffer.Empty();
	CachedDestinationBuffer.Add(FVector::ZeroVector);
}

void Asteering_player_controller::DrawPath() const
{
	for (int i = 0; i < CachedDestinationBuffer.Num(); i++)
		DrawSphere(CachedDestinationBuffer[i], 15, (i <= 1) ? FColor::Red : FColor::Emerald);
}

void Asteering_player_controller::DrawSphere(const FVector& Center, const float Radius, const FColor Color) const
{
	DrawDebugSphere(GetWorld(), Center, Radius, 12, Color, false, 0.f);
}

void Asteering_player_controller::DrawLine(const FVector& LineStart, const FVector& LineEnd, const FColor Color) const
{
	DrawDebugLine(GetWorld(), LineStart, LineEnd, Color, false, 0.f, 0, 2.f);
}
