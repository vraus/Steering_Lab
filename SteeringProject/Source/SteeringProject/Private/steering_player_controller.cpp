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
	behaviour = EBehaviours::Undefined;
}

void Asteering_player_controller::SetMovementBehaviour(EBehaviours new_behaviour)
{
	behaviour = new_behaviour;
	UE_LOG(LogTemp, Display, TEXT("New Behaviour received"));
}

void Asteering_player_controller::SetCharacter(Asteering_character* player_pawn)
{
	if (player_pawn)
		character_ = player_pawn;
	else {
		UE_LOG(LogTemp, Error, TEXT("Error: invalid player_pawn."));
	}
}

void Asteering_player_controller::BeginPlay()
{
	Super::BeginPlay();
}

void Asteering_player_controller::OnInputStarted()
{
	StopMovement();
}

void Asteering_player_controller::MoveTo(FVector Location)
{
	switch (behaviour)
	{
	case EBehaviours::Undefined:
		UE_LOG(LogTemp, Log, TEXT("Undefined"));
		break;
	case EBehaviours::Seek:
		MoveSeek(Location);
		break;
	case EBehaviours::Flee:
		MoveFlee(Location);
		break;
	case EBehaviours::Pursuit:
		MovePursuit(Location);
		break;
	case EBehaviours::Evade:
		MoveEvade(Location);
		break;
	case EBehaviours::Arrival:
		MoveArrival(Location);
		break;
	default:
		UE_LOG(LogTemp, Log, TEXT("Default"));
		break;
	}
}

void Asteering_player_controller::MoveSeek(FVector Location)
{
	UE_LOG(LogTemp, Log, TEXT("Seek %s"), *Location.ToString());

	FVector character_location = character_->GetActorLocation();
	FVector character_forward = character_->GetActorForwardVector();

	UE_LOG(LogTemp, Log, TEXT("\t%s"), *character_location.ToString());
	UE_LOG(LogTemp, Log, TEXT("\t%s"), *character_forward.ToString());

	FVector move_to_vector = UKismetMathLibrary::GetDirectionUnitVector(character_location, Location);

	character_->SetActorLocation(Location);
}

void Asteering_player_controller::MoveFlee(FVector Location)
{
	UE_LOG(LogTemp, Log, TEXT("Flee %s"), *Location.ToString());
}

void Asteering_player_controller::MovePursuit(FVector Location)
{
	UE_LOG(LogTemp, Log, TEXT("Pursuit %s"), *Location.ToString());
}

void Asteering_player_controller::MoveEvade(FVector Location)
{
	UE_LOG(LogTemp, Log, TEXT("Evade %s"), *Location.ToString());
}

void Asteering_player_controller::MoveArrival(FVector Location)
{
	UE_LOG(LogTemp, Log, TEXT("Arrival %s"), *Location.ToString());
}
