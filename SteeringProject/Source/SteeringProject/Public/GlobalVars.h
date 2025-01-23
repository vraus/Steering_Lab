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
};

/**
* Struct containing all the infos relative to the movement speed and direction of the actor
*/
USTRUCT(BlueprintType)
struct FMovableActorInfos
{
	GENERATED_USTRUCT_BODY()


	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float Mass;

	/** For now I don't need this...
		UPROPERTY(EditAnywhere, BlueprintReadWrite)
		FVector Position;

		UPROPERTY(EditAnywhere, BlueprintReadWrite)
		FVector Velocity;

		UPROPERTY(EditAnywhere, BlueprintReadWrite)
		FRotator Orientation;
	*/

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float MaxSpeed;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float MaxForce;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float RotationSpeed;
};
