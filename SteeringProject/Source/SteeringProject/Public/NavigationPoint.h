// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Structs/SNavNode.h"
#include "NavigationPoint.generated.h"

UCLASS(Blueprintable, ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class STEERINGPROJECT_API ANavigationPoint : public AActor
{
	GENERATED_BODY()

public:
	ANavigationPoint();

	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;

	UFUNCTION(BlueprintCallable, Category = "Navigation|Costs")
	float FCost() const { return NavNode.GCost + NavNode.HCost; }

	UFUNCTION(BlueprintCallable, Category = "Navigation|Navigation Node")
	TArray<ANavigationPoint*> GetNeighbors() const { return ConnectedPoints; }
	
protected:

	UFUNCTION(BlueprintCallable, Category = "Navigation")
	void FindNearbyNavPoints();

	UFUNCTION(BlueprintCallable, Category = "Navigation")
	bool CheckIfCanConnect(const AActor* PotentialNeighbor) const;
	
private:

	UFUNCTION(BlueprintCallable, Category="Debug")
	void DrawConnections() const;

public:
	UPROPERTY(BlueprintReadWrite, VisibleAnywhere, Category = "Components")
	USceneComponent* Root;

	UPROPERTY(BlueprintReadWrite, VisibleAnywhere, Category = "Navigation Node")
	FSNavNode NavNode;
	
protected:
	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Category = "Navigation")
	TArray<ANavigationPoint*> ConnectedPoints;

	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "Navigation")
	float ConnectionRadius;
};
