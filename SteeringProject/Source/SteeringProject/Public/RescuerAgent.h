// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "NavigationPoint.h"
#include "TargetCharacter.h"
#include "GameFramework/Character.h"
#include "RescuerAgent.generated.h"

UCLASS(Blueprintable, BlueprintType, ClassGroup=(Custom))
class STEERINGPROJECT_API ARescuerAgent : public ACharacter
{
	GENERATED_BODY()

public:
	ARescuerAgent();

protected:
	virtual void BeginPlay() override;

	virtual void Tick(float DeltaTime) override;

	UFUNCTION(BlueprintCallable, Category = "Navigation")
	ANavigationPoint* FindClosestNavPoint(const AActor* Target) const;

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Navigation|Initialisation")
	ATargetCharacter* TargetCharacter;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Navigation|Initialisation")
	ANavigationPoint* StartPoint;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Navigation|Initialisation")
	ANavigationPoint* EndPoint;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Navigation|Initialisation")
	bool bIsFindingPath;

private:

	// Debug Function
	void DrawDebugPath(const AActor* StartingPoint, const AActor* EndingPoint) const;
};