// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "steering_character.h"
#include "steering_rescuer.generated.h"

UCLASS()
class STEERINGPROJECT_API Asteering_rescuer : public Asteering_character
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	Asteering_rescuer();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UFUNCTION(BlueprintCallable, Category = "Rescuer")
	void MoveTo();

	UFUNCTION(BlueprintCallable, Category = "Rescuer")
	void SetCachedDestination(const FVector& NewDestination);

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

private:
	void MoveSeek();
	void MoveArrival();
	void PathOneWay();

	FVector Velocity;

	FVector CachedDestination;
	TArray<FVector> CachedDestinationBuffer;

	bool bShouldMove;

	// Draw debugs
	void DrawPath() const;
	void DrawSphere(const FVector& Center, const float Radius, const FColor Color) const;
	void DrawLine(const FVector& LineStart, const FVector& LineEnd, const FColor Color) const;
};
