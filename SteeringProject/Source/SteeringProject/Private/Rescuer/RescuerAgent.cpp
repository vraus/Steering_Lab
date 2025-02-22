// Fill out your copyright notice in the Description page of Project Settings.


#include "Rescuer/RescuerAgent.h"


// Sets default values
ARescuerAgent::ARescuerAgent()
{
	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
}

// Called when the game starts or when spawned
void ARescuerAgent::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ARescuerAgent::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

// Called to bind functionality to input
void ARescuerAgent::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
}

