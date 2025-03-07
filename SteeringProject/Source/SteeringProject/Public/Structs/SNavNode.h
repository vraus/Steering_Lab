// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "SNavNode.generated.h"

class ANavigationPoint;

USTRUCT(Blueprintable)
struct FSNavNode
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadWrite, Category = "Navigation Node")
	ANavigationPoint *Parent;

	UPROPERTY(BlueprintReadWrite, Category = "Navigation Node")
	float GCost;

	UPROPERTY(BlueprintReadWrite, Category = "Navigation Node")
	float HCost;

	UPROPERTY(BlueprintReadWrite, Category = "Navigation Node")
	float FCost;
};