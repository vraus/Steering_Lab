// Fill out your copyright notice in the Description page of Project Settings.

#include "steering_character.h"

// Sets default values
Asteering_character::Asteering_character()
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

	bUseControllerRotationYaw = false;

}

// Called when the game starts or when spawned
void Asteering_character::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void Asteering_character::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

// Called to bind functionality to input
void Asteering_character::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}

