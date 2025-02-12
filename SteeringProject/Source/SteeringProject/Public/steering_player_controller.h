// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

// Project
#include "GlobalVars.h"
#include "steering_character.h"
#include "TargetActor.h"

// Core
#include "CoreMinimal.h"
#include "TargetCharacter.h"
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
	void SetMovementBehaviour(EBehaviours New_Behaviour);

	UFUNCTION(BlueprintCallable, Category = "Steering")
	void SetCharacter(Asteering_character* Player_Pawn);

	UFUNCTION(BlueprintCallable, Category = "Steering")
	void SetTargetCharacter(ATargetCharacter* Target_Character);

	/** Time Threshold to know if it was a short press */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input)
	float ShortPressThreshold;

	UFUNCTION(BlueprintCallable, Category = "Steering")
	void OnInputStarted();

	UFUNCTION(BlueprintCallable, Category = "Steering")
	void OnGoPath();

	UFUNCTION(BlueprintCallable, Category = "Steering")
	void UpdatePlayerInfos(FMovableActorInfos NewPlayerInfos) { Player_Stats = NewPlayerInfos; }

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

	virtual void Tick(float DeltaSeconds) override;

	/** Movement functions
	* Make Character move to Cached Destination
	*/
	UFUNCTION(BlueprintCallable, Category = "Steering")
	void MoveTo();

	EBehaviours Behaviour;

	UFUNCTION(BlueprintCallable, Category = "Steering")
	void SetCachedLocation(const FVector& NewLocation);

private:
	void MoveSeek() const;
	void MoveFlee() const;
	void MovePursuit() const;
	void MoveEvade();
	void MoveArrival();
	void PathCircuit();
	void PathOneWay();
	void PathTwoWay();

	void ResetCachedDestinationBuffer();

	ATargetCharacter *TargetCharacter;
	Asteering_character *character_;

	FMovableActorInfos Player_Stats;

	FVector Velocity;
	FVector CachedDestination;
	TArray<FVector> CachedDestinationBuffer;
	FVector TargetCharacter_Location;
	FVector TargetCharacter_Velocity;

	bool bPathMode;
	bool bShouldMove;
	float FollowTime;
	float LastEvadeTime;

	/** Draw Debug Functions */	
	void DrawPath() const;
	void DrawSphere(const FVector& Center, const float Radius, const FColor Color) const;
	void DrawLine(const FVector& LineStart, const FVector& LineEnd, const FColor Color) const;
};
