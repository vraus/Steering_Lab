// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

// Project
#include "GlobalVars.h"
#include "steering_character.h"

// Core
#include "CoreMinimal.h"
#include "Templates/SubclassOf.h"
#include "GameFramework/PlayerController.h"
#include "steering_player_controller.generated.h"

class UInputMappingContext;
class UInputAction;

UCLASS()
class STEERINGPROJECT_API Asteering_player_controller : public APlayerController
{
	GENERATED_BODY()

public:
	Asteering_player_controller();

	UFUNCTION(BlueprintCallable, Category = "Steering")
	void SetMovementBehaviour(EBehaviours new_behaviour);

	UFUNCTION(BlueprintCallable, Category = "Steering")
	void SetCharacter(Asteering_character* player_pawn);

	/** Time Threshold to know if it was a short press */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input)
	float ShortPressThreshold;

	/** MappingContext */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category=Input, meta=(AllowPrivateAccess = "true"))
	UInputMappingContext* DefaultMappingContext;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category=Input, meta=(AllowPrivateAccess = "true"))
	UInputAction* SetDestinationClickAction;
	
protected:
	/** True if the controlled character should navigate to the mouse cursor. */
	bool bMoveToMouseCursor;
	
	// To add mapping context
	virtual void BeginPlay() override;

	virtual void Tick() override;

	/** Input handlers for SetDestination action. */
	void OnInputStarted();

	/** Movement functions */
	UFUNCTION(BlueprintCallable, Category = "Steering")
	void MoveTo(FVector Location);

	EBehaviours behaviour;

private:
	void MoveSeek(FVector Location);
	void MoveFlee(FVector Location);
	void MovePursuit(FVector Location);
	void MoveEvade(FVector Location);
	void MoveArrival(FVector Location);

	Asteering_character *character_;
	FVector CachedDestination;
	float FollowTime;
};
