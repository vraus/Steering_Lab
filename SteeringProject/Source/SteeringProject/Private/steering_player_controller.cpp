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
	FollowTime = 0.f;
	Behaviour = EBehaviours::Undefined;

	bShouldMove = false;
	Velocity = FVector::ZeroVector;
}

void Asteering_player_controller::SetMovementBehaviour(EBehaviours New_Behaviour)
{
	Behaviour = New_Behaviour;

	// Unsure that the character start chasing or evading when New behavior set to said behaviors, this even if bShouldMove is false meaning the player is not curently moving.
	switch (Behaviour)
	{
	case EBehaviours::Pursuit:
		bShouldMove = true;
		break;
	case EBehaviours::Evade:
		bShouldMove = true;
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
	else {
		TargetCharacter_Location = FVector::ZeroVector;
		TargetCharacter_Velocity = FVector::ZeroVector;
	}

	MoveTo();
}

void Asteering_player_controller::OnInputStarted()
{
	StopMovement();
	bShouldMove = true;

	UE_LOG(LogTemp, Log, TEXT("Input Started"));
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
	default:
		UE_LOG(LogTemp, Log, TEXT("Default"));
		break;
	}
}

void Asteering_player_controller::MoveSeek()
{
	FVector DesiredVelocity = (CachedDestination - character_->GetActorLocation()).GetSafeNormal() * Player_Stats.MaxSpeed;

	FVector Steering = DesiredVelocity - character_->GetVelocity();

	FRotator TargetRotation = DesiredVelocity.Rotation();
	TargetRotation.Roll = 0.f;
	TargetRotation.Pitch = 0.f;

	FRotator CurrentRotation = character_->GetActorRotation();
	FRotator SmoothedRotation = FMath::RInterpTo(CurrentRotation, TargetRotation, GetWorld()->GetDeltaSeconds(), Player_Stats.RotationSpeed);

	character_->SetActorRotation(SmoothedRotation);

	character_->AddMovementInput(SmoothedRotation.Vector(), Steering.Size(), true);

	DrawDebugLine(GetWorld(), character_->GetActorLocation(), CachedDestination, FColor::Blue, false, 0.1f, 0, 2.0f);
	DrawDebugSphere(GetWorld(), CachedDestination, 50, 12, FColor::Red, false, 0.1f);
}

void Asteering_player_controller::MoveFlee()
{
	FVector DesiredVelocity = (character_->GetActorLocation() - CachedDestination).GetSafeNormal() * Player_Stats.MaxSpeed;

	FVector Steering = DesiredVelocity - character_->GetVelocity();

	FRotator TargetRotation = DesiredVelocity.Rotation();
	TargetRotation.Roll = 0.f;
	TargetRotation.Pitch = 0.f;

	FRotator CurrentRotation = character_->GetActorRotation();
	FRotator SmoothedRotation = FMath::RInterpTo(CurrentRotation, TargetRotation, GetWorld()->GetDeltaSeconds(), Player_Stats.RotationSpeed);

	character_->SetActorRotation(SmoothedRotation);

	character_->AddMovementInput(SmoothedRotation.Vector(), Steering.Size(), true);

	DrawDebugLine(GetWorld(), character_->GetActorLocation(), CachedDestination, FColor::Blue, false, 0.1f, 0, 2.0f);
	DrawDebugSphere(GetWorld(), CachedDestination, 50, 12, FColor::Red, false, 0.1f);
}

void Asteering_player_controller::MovePursuit()
{
	FVector Target_FuturLocation = TargetCharacter_Location + TargetCharacter_Velocity / 5;

	FVector DesiredVelocity = (Target_FuturLocation - character_->GetActorLocation()).GetSafeNormal() * Player_Stats.MaxSpeed;

	FVector Steering = DesiredVelocity - character_->GetVelocity();

	FRotator TargetRotation = DesiredVelocity.Rotation();
	TargetRotation.Roll = 0.f;
	TargetRotation.Pitch = 0.f;

	FRotator CurrentRotation = character_->GetActorRotation();
	FRotator SmoothedRotation = FMath::RInterpTo(CurrentRotation, TargetRotation, GetWorld()->GetDeltaSeconds(), Player_Stats.RotationSpeed);

	character_->SetActorRotation(SmoothedRotation);

	character_->AddMovementInput(SmoothedRotation.Vector(), Steering.Size(), true);

	DrawDebugLine(GetWorld(), character_->GetActorLocation(), Target_FuturLocation, FColor::Blue, false, 0.1f, 0, 2.0f);
	DrawDebugSphere(GetWorld(), Target_FuturLocation, 50, 12, FColor::Red, false, 0.1f);
}

void Asteering_player_controller::MoveEvade()
{
	FVector Target_FuturLocation = TargetCharacter_Location + TargetCharacter_Velocity / 5;

	FVector DesiredVelocity = (character_->GetActorLocation() - Target_FuturLocation).GetSafeNormal() * Player_Stats.MaxSpeed;

	FVector Steering = DesiredVelocity - character_->GetVelocity();

	FRotator TargetRotation = DesiredVelocity.Rotation();
	TargetRotation.Roll = 0.f;
	TargetRotation.Pitch = 0.f;

	FRotator CurrentRotation = character_->GetActorRotation();
	FRotator SmoothedRotation = FMath::RInterpTo(CurrentRotation, TargetRotation, GetWorld()->GetDeltaSeconds(), Player_Stats.RotationSpeed);

	character_->SetActorRotation(SmoothedRotation);

	character_->AddMovementInput(SmoothedRotation.Vector(), Steering.Size(), true);

	DrawDebugLine(GetWorld(), character_->GetActorLocation(), Target_FuturLocation, FColor::Blue, false, 0.1f, 0, 2.0f);
	DrawDebugSphere(GetWorld(), Target_FuturLocation, 50, 12, FColor::Red, false, 0.1f);
}

void Asteering_player_controller::MoveArrival()
{
	FVector TargetOffset = CachedDestination - character_->GetActorLocation();
	float Distance = TargetOffset.Size();

	if (Distance <= Player_Stats.StoppingDistance)
	{
		Velocity = FVector::ZeroVector;
		bShouldMove = false;
		return;
	}

	float RampedSpeed = Player_Stats.MaxSpeed * (Distance / Player_Stats.SlowingDistance);
	float ClippedSpeed = FMath::Min(RampedSpeed, Player_Stats.MaxSpeed);

	FVector DesiredVelocity = TargetOffset.GetSafeNormal() * ClippedSpeed;

	FVector Steering = DesiredVelocity - character_->GetVelocity();

	FRotator TargetRotation = DesiredVelocity.Rotation();
	TargetRotation.Roll = 0.f;
	TargetRotation.Pitch = 0.f;

	FRotator CurrentRotation = character_->GetActorRotation();
	FRotator SmoothedRotation = FMath::RInterpTo(CurrentRotation, TargetRotation, GetWorld()->GetDeltaSeconds(), Player_Stats.RotationSpeed);

	character_->SetActorRotation(SmoothedRotation);

	character_->AddMovementInput(SmoothedRotation.Vector(), Steering.Size(), true);

	DrawDebugLine(GetWorld(), character_->GetActorLocation(), CachedDestination, FColor::Blue, false, 0.1f, 0, 2.0f);
	DrawDebugSphere(GetWorld(), CachedDestination, Player_Stats.StoppingDistance, 12, FColor::Red, false, 0.1f);
}
