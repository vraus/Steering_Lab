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

void Asteering_player_controller::BeginPlay()
{
	Super::BeginPlay();
}

void Asteering_player_controller::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	if (!bShouldMove) return;

	MoveTo(CachedDestination, DeltaSeconds);
}

void Asteering_player_controller::OnInputStarted()
{
	StopMovement();
	bShouldMove = true;

	UE_LOG(LogTemp, Log, TEXT("Input Started"));
}

void Asteering_player_controller::MoveTo(const FVector Target_Location, float DeltaSeconds)
{
	switch (Behaviour)
	{
	case EBehaviours::Undefined:
		UE_LOG(LogTemp, Error, TEXT("Undefined"));
		break;
	case EBehaviours::Seek:
		MoveSeek(Target_Location, DeltaSeconds);
		break;
	case EBehaviours::Flee:
		MoveFlee(Target_Location, DeltaSeconds);
		break;
	case EBehaviours::Pursuit:
		MovePursuit(Target_Location, DeltaSeconds);
		break;
	case EBehaviours::Evade:
		MoveEvade(Target_Location, DeltaSeconds);
		break;
	case EBehaviours::Arrival:
		MoveArrival(Target_Location, DeltaSeconds);
		break;
	default:
		UE_LOG(LogTemp, Log, TEXT("Default"));
		break;
	}
}

void Asteering_player_controller::MoveSeek(const FVector& Target_Location, const float DeltaSeconds)
{
	const FVector Steering_Direction = (Target_Location - character_->GetActorLocation()).GetSafeNormal();
	
	FVector Steering_Force = Steering_Direction * Player_Stats.MaxForce;
	Steering_Force = Steering_Force.GetClampedToMaxSize(Player_Stats.MaxForce);

	const FVector Acceleration = Steering_Force / Player_Stats.Mass;
	
	Velocity += Acceleration * DeltaSeconds;
	Velocity = Velocity.GetClampedToMaxSize(Player_Stats.MaxSpeed);
	
	character_->AddMovementInput(character_->GetActorForwardVector(), Velocity.Size(), true);

	if (!Velocity.IsNearlyZero())
	{
		FRotator TargetRotation = Velocity.Rotation();
		const FRotator CurrentRotation = character_->GetActorRotation();

		TargetRotation.Roll = 0.f;
		TargetRotation.Pitch = 0.f;
		
		const FRotator SmoothedRotation = FMath::RInterpTo(CurrentRotation, TargetRotation, DeltaSeconds, Player_Stats.RotationSpeed);

		character_->SetActorRotation(SmoothedRotation);
	}

}

void Asteering_player_controller::MoveFlee(const FVector& Target_Location, float DeltaSeconds)
{
	const FVector Steering_Direction = (Target_Location - character_->GetActorLocation()).GetSafeNormal();

	FVector Steering_Force = Steering_Direction * Player_Stats.MaxForce * -1;
	Steering_Force = Steering_Force.GetClampedToMaxSize(Player_Stats.MaxForce);

	const FVector Acceleration = Steering_Force / Player_Stats.Mass;

	Velocity += Acceleration * DeltaSeconds;
	Velocity = Velocity.GetClampedToMaxSize(Player_Stats.MaxSpeed);

	character_->AddMovementInput(character_->GetActorForwardVector(), Velocity.Size(), true);

	if (!Velocity.IsNearlyZero())
	{
		FRotator TargetRotation = Velocity.Rotation();
		const FRotator CurrentRotation = character_->GetActorRotation();

		TargetRotation.Roll = 0.f;
		TargetRotation.Pitch = 0.f;

		const FRotator SmoothedRotation = FMath::RInterpTo(CurrentRotation, TargetRotation, DeltaSeconds, Player_Stats.RotationSpeed);

		character_->SetActorRotation(SmoothedRotation);
	}
}

void Asteering_player_controller::MovePursuit(const FVector& Target_Location, float DeltaSeconds)
{
	UE_LOG(LogTemp, Log, TEXT("Pursuit %s"), *Target_Location.ToString());
}

void Asteering_player_controller::MoveEvade(const FVector& Target_Location, float DeltaSeconds)
{
	UE_LOG(LogTemp, Log, TEXT("Evade %s"), *Target_Location.ToString());
}

void Asteering_player_controller::MoveArrival(const FVector& Target_Location, float DeltaSeconds)
{
	FVector TargetOffset = FVector(Target_Location.X, Target_Location.Y, 0.f) - FVector(character_->GetActorLocation().X, character_->GetActorLocation().Y, 0.f);
	float Distance = TargetOffset.Size();

	UE_LOG(LogTemp, Log, TEXT("Distance: %f <? StopDistance :%f"), Distance, Player_Stats.StoppingDistance);

	// Vérifie si la distance est inférieure à un seuil pour s'arrêter complètement
	if (Distance < Player_Stats.StoppingDistance)
	{
		Velocity = FVector::ZeroVector;
		bShouldMove = false; // Arrêt du mouvement
		return;
	}

	// Calculer la vitesse rampée et la vitesse souhaitée
	float AnticipationFactor = 1.5f;
	float EffectiveSlowingDistance = Player_Stats.SlowingDistance * AnticipationFactor;

	float SpeedFactor = FMath::Square(Distance / EffectiveSlowingDistance);
	float ClippedSpeed = Player_Stats.MaxSpeed * FMath::Clamp(SpeedFactor, 0.0f, 1.0f);

	FVector DesiredVelocity = TargetOffset.GetSafeNormal() * ClippedSpeed;
	FVector Steering = DesiredVelocity - Velocity;
	FVector Steering_Force = Steering.GetClampedToMaxSize(Player_Stats.MaxForce);
	FVector Acceleration = Steering_Force / Player_Stats.Mass;

	Velocity += Acceleration * DeltaSeconds;
	Velocity = Velocity.GetClampedToMaxSize(Player_Stats.MaxSpeed);

	// Déplacement du personnage
	character_->AddMovementInput(character_->GetActorForwardVector(), Velocity.Size(), true);

	// Rotation smooth du personnage
	if (!Velocity.IsNearlyZero())
	{
		FRotator TargetRotation = Velocity.Rotation();
		FRotator CurrentRotation = character_->GetActorRotation();

		TargetRotation.Roll = 0.f;
		TargetRotation.Pitch = 0.f;

		FRotator SmoothedRotation = FMath::RInterpTo(CurrentRotation, TargetRotation, DeltaSeconds, Player_Stats.RotationSpeed);
		character_->SetActorRotation(SmoothedRotation);
	}
}
