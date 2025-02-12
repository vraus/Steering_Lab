// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "GlobalVars.generated.h"

/**
 * Enum containing all the Behaviours we want for our character through the game.
 */
UENUM (BlueprintType)
enum class EBehaviours : uint8
{
	Undefined UMETA(DisplayName = "Undefined"),
	Seek UMETA(DisplayName = "Seek"),
	Flee UMETA(DisplayName = "Flee"),
	Pursuit UMETA(DisplayName = "Pursuit"),
	Evade UMETA(DisplayName = "Evade"),
	Arrival UMETA(DisplayName = "Arrival"),
	Circuit UMETA(DisplayName = "Circuit"),
	OneWay UMETA(DisplayName = "OneWay"),
	TwoWay UMETA(DisplayName = "TwoWay"),
};

UENUM(BlueprintType)
enum class ESliders : uint8
{
	Undefined UMETA(DisplayName = "Undefined"),
	RotationSpeed UMETA(DisplayName = "Rotation Speed"),
	PursuitPrediction UMETA(DisplayName = "Pursuit Prediction"),
	FleeThreshold UMETA(DisplayName = "Flee Threshold"),
	EvadeCooldown UMETA(DisplayName = "Evade Cooldown"),
};

/**
* Struct containing all the infos relative to the movement speed and direction of the actor
*/
USTRUCT(BlueprintType)
struct FMovableActorInfos
{
	GENERATED_USTRUCT_BODY()

	FMovableActorInfos()
		: Mass(1.f)
		, MaxSpeed(600.f)
		, MaxForce(500.f)
		, RotationSpeed(5.0f)
		, SlowingDistance(300.0f)
		, StoppingDistance(50.0f)
		, FleeThreshold(200.0f)
		, PursuitPrediction(1.0f)
		, EvadeCooldown(2.0f)
		, ValidatePathPointThreshold(10.0f)
	{}
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float Mass;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float MaxSpeed;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float MaxForce;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float RotationSpeed;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float SlowingDistance;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float StoppingDistance;

	/** When the character flees, and the target character reaches this threshold distance, the character will change direction. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float FleeThreshold;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float PursuitPrediction;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float EvadeCooldown;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float ValidatePathPointThreshold;
};
