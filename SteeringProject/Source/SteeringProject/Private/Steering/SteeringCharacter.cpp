// Fill out your copyright notice in the Description page of Project Settings.

#include "Steering/SteeringCharacter.h"

// Sets default values
ASteeringCharacter::ASteeringCharacter()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	ActorInfos.Mass = 1.f;
	ActorInfos.MaxSpeed = 600.f;
	ActorInfos.MaxForce = 600.f;
	ActorInfos.RotationSpeed = 6.f;
	ActorInfos.SlowingDistance = 600.f;
	ActorInfos.StoppingDistance = 50.f;
	ActorInfos.FleeThreshold = 90.f;
	ActorInfos.EvadeCooldown = .25f;
	ActorInfos.EvadeCooldown = 2.f;
	ActorInfos.ValidatePathPointThreshold = 150.f;

	bUseControllerRotationYaw = false;

}

// Called when the game starts or when spawned
void ASteeringCharacter::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ASteeringCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

// Called to bind functionality to input
void ASteeringCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}

