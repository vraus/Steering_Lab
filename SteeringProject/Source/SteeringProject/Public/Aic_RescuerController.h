// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "steering_character.h"
#include "Aic_RescuerController.generated.h"

/**
 * 
 */
UCLASS()
class STEERINGPROJECT_API AAic_RescuerController : public AAIController
{
	GENERATED_BODY()

public:
	AAic_RescuerController();

protected:

	virtual void Tick(float DeltaTime) override;

	UFUNCTION(BlueprintCallable, Category="AI Controller|Path")
	void AddDestinationToCache(FVector NewDestination);

private:
	void MoveTo();
	void MoveSeek() const;

	bool bShouldMove;

	UPROPERTY()
	Asteering_character* Agent;

	FVector Velocity;
	FVector CachedDestination;
	TArray<FVector> CachedDestinationBuffer;

	/** Draw Debug Functions */
	void DrawPath() const;
	void DrawSphere(const FVector& Center, const float Radius, const FColor Color) const;
	void DrawLine(const FVector& LineStart, const FVector& LineEnd, const FColor Color) const;
	
};
