// Fill out your copyright notice in the Description page of Project Settings.


#include "TargetCharacter.h"


// Sets default values
ATargetCharacter::ATargetCharacter()
{
	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
}

// Called when the game starts or when spawned
void ATargetCharacter::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ATargetCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

// Called to bind functionality to input
void ATargetCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
}

