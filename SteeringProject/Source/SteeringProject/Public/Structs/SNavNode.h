// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "SNavNode.generated.h"

USTRUCT(Blueprintable)
struct FSNavNode
{
	GENERATED_BODY()

	/** Current point */
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	AActor* NavPoint;

	/** Cost of NavPoint from start */
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float GCost;

	/** Heuristic distance from target */
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float HCost;

	/** Total cost for this point */
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float FCost;

	/** Parent point of this point to build the navigation */
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	AActor* ParentPoint;
};
