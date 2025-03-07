// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "NavigationPoint.h"
#include "TargetCharacter.h"
#include "GameFramework/Character.h"
#include "RescuerAgent.generated.h"

UCLASS(Blueprintable, BlueprintType, ClassGroup=(Custom))
class STEERINGPROJECT_API ARescuerAgent : public ACharacter
{
	GENERATED_BODY()

public:
	ARescuerAgent();

protected:
	virtual void BeginPlay() override;

	virtual void Tick(float DeltaTime) override;

	UFUNCTION(BlueprintCallable, Category = "Navigation|Initialisation")
	ANavigationPoint* FindClosestNavPoint(const AActor* Target) const;

	UFUNCTION(BlueprintCallable, Category = "Navigation|Initialisation")
	void StartPathFinding();

	UFUNCTION(BlueprintCallable, Category = "Navigation|Path")
	void FindPath();

	UFUNCTION(BlueprintCallable, Category = "Navigation|Path")
	void StartPathReconstruction();

	UFUNCTION(BlueprintCallable, Category = "Navigation|Path")
	void ReconstructPath();

	UFUNCTION(BlueprintCallable, Category = "Navigation|Path")
	void MoveAlongPath();

	UFUNCTION(BlueprintCallable, Category = "Navigation|Costs")
	ANavigationPoint* GetLowestFCost() const;

protected:
	UPROPERTY(VisibleDefaultsOnly, BlueprintReadWrite, Category = "Navigation|Initialisation")
	ATargetCharacter* TargetCharacter;
	
	UPROPERTY(VisibleDefaultsOnly, BlueprintReadWrite, Category = "Navigation|Initialisation")
	ANavigationPoint* StartPoint;

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadWrite, Category = "Navigation|Initialisation")
	ANavigationPoint* EndPoint;

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadWrite, Category = "Navigation|Initialisation")
	bool bIsFindingPath;

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadWrite, Category = "Navigation|Queues")
	TArray<ANavigationPoint*> OpenQueue;

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadWrite, Category = "Navigation|Queues")
	TArray<ANavigationPoint*> ClosedQueue;

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadWrite, Category = "Navigation|Path")
	TArray<ANavigationPoint*> Path;

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadWrite, Category = "Navigation|Path")
	ANavigationPoint* CurrentPathNode;	

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadWrite, Category = "Navigation|Path")
	bool bIsBuildingPath;

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadWrite, Category = "Navigation|Path")
	bool bCanMove;

private:
	void ClearQueues();

	// Debug Function
	void DrawDebugPath(const AActor* StartingPoint, const AActor* EndingPoint) const;
};