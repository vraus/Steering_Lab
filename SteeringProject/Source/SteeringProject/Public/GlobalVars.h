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
