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
		UE_LOG(LogTemp, Log, TEXT("Undefined"));
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

void Asteering_player_controller::MoveSeek(const FVector& Target_Location, float DeltaSeconds)
{
	UE_LOG(LogTemp, Log, TEXT("Seek %s"), *Target_Location.ToString());

	const FVector Steering_Direction = (Target_Location - character_->GetActorLocation()).GetSafeNormal();
	
	FVector Steering_Force = Steering_Direction * Player_Stats.MaxForce;
	Steering_Force = Steering_Force.GetClampedToMaxSize(Player_Stats.MaxForce);

	const FVector Acceleration = Steering_Force / Player_Stats.Mass;
	
	Velocity += Acceleration * DeltaSeconds;
	Velocity = Velocity.GetClampedToMaxSize(Player_Stats.MaxSpeed);
	
	const FVector ForwardDirection = character_->GetActorForwardVector();

	const FVector NewPosition = character_->GetActorLocation() + ForwardDirection * Velocity.Size() * DeltaSeconds;
	character_->SetActorLocation(NewPosition);

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
	UE_LOG(LogTemp, Log, TEXT("Flee %s"), *Target_Location.ToString());
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
	UE_LOG(LogTemp, Log, TEXT("Arrival %s"), *Target_Location.ToString());
}
