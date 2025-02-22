// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

// Project
#include "GlobalVars.h"

// Core
#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "SteeringCharacter.generated.h"

UCLASS()
class STEERINGPROJECT_API ASteeringCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	ASteeringCharacter();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Steering")
	FMovableActorInfos ActorInfos;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	FMovableActorInfos GetActorInfos() const { return ActorInfos;  }
private:
};
