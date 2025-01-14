// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Templates/SubclassOf.h"
#include "GameFramework/PlayerController.h"
#include "steering_player_controller.generated.h"

class UInputMappingContext;
class UInputAction;

DECLARE_LOG_CATEGORY_EXTERN(LogTemplateCharacter, Log, All);

UCLASS()
class STEERINGPROJECT_API Asteering_player_controller : public APlayerController
{
	GENERATED_BODY()

public:
	Asteering_player_controller();

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

	/** Input handlers for SetDestination action. */
	void OnInputStarted();

	/** Movement functions */
	UFUNCTION(BlueprintCallable, Category = "Steering")
	void MoveTo(FVector Location);

private:
	FVector CachedDestination;

	float FollowTime; // For how long it has been pressed
};
