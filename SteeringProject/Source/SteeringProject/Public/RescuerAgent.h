// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GlobalVars.h"
#include "NavigationPoint.h"
#include "GameFramework/Character.h"
#include "RescuerAgent.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnTargetReached, ARescuerAgent*, Instigator, bool, bTargetWasCharacter);

UCLASS(Blueprintable, BlueprintType, ClassGroup=(Custom))
class STEERINGPROJECT_API ARescuerAgent : public ACharacter
{
	GENERATED_BODY()

public:
	ARescuerAgent();

	UFUNCTION(BlueprintCallable, Category= "Navigation|Initialisation")
	void InitTarget(AActor* Target, const bool bIsCharacter);

	UFUNCTION(BlueprintCallable, Category= "Navigation|Initialisation")
	void Ready();

	UPROPERTY(BlueprintAssignable, Category= "Navigation|Path")
	FOnTargetReached OnTargetReached;
	
protected:
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
	FVector DetectAgentInFront() const;
	
	UFUNCTION(BlueprintCallable, Category = "Navigation|Path")
	void MoveAlongPath();

	UFUNCTION(BlueprintCallable, Category = "Navigation|Path")
	void PathOneWay();

	UFUNCTION(BlueprintCallable, Category = "Navigation|Path")
	void MoveSeek();

	UFUNCTION(BlueprintCallable, Category = "Navigation|Path")
	void MoveArrival();

	UFUNCTION(BlueprintCallable, Category = "Navigation|Costs")
	ANavigationPoint* GetLowestFCost() const;

protected:
	/** Initialisation */
	
	UPROPERTY(VisibleDefaultsOnly, BlueprintReadWrite, Category = "Navigation|Initialisation")
	AActor* TargetCharacter;
	
	UPROPERTY(VisibleDefaultsOnly, BlueprintReadWrite, Category = "Navigation|Initialisation")
	ANavigationPoint* StartPoint;

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadWrite, Category = "Navigation|Initialisation")
	ANavigationPoint* EndPoint;
	
	/** Queues */
	
	UPROPERTY(VisibleDefaultsOnly, BlueprintReadWrite, Category = "Navigation|Queues")
	TArray<ANavigationPoint*> OpenQueue;

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadWrite, Category = "Navigation|Queues")
	TArray<ANavigationPoint*> ClosedQueue;

	/** Data */
	UPROPERTY(VisibleDefaultsOnly, BlueprintReadWrite, Category = "Navigation|Datas")
	TMap<ANavigationPoint*, FSNavNode> NavData;
	
	/** Path */
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Navigation|Path")
	TArray<ANavigationPoint*> Path;

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadWrite, Category = "Navigation|Path")
	ANavigationPoint* CurrentPathNode;

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadWrite, Category = "Navigation|Path")
	bool bTargetIsCharacter;

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadWrite, Category = "Navigation|Path")
	FVector CachedLocation;

	/** Movements */
	UPROPERTY(VisibleDefaultsOnly, BlueprintReadWrite, Category = "Navigation|Movement")
	FVector Velocity;

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = "Navigation|Initialisation")
	ERescuerStates State;
	
public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Infos")
	FMovableActorInfos Player_Stats;

private:
	void ClearQueues();

	// Debug Function
	void DrawDebugPath() const;
	void DrawLine(const AActor* StartingPoint, const AActor* EndingPoint, FColor Color) const;
	void DrawSphere(const FVector& Center, const float Radius, const FColor Color) const;
	void DebugClosedQueue() const;
	void DebugOpenQueue() const;
};