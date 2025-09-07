// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "TurretAIController.generated.h"

/**
 * 
 */
UCLASS()
class TOONTANKS_API ATurretAIController : public AAIController
{
	GENERATED_BODY()
	


public:
	virtual void Tick(float DeltaSeconds) override;
	virtual void OnPossess(APawn* InPawn) override;

private:
	bool InFireRange();

	class ATank* Tank;
	class ATower* Tower;

	UPROPERTY(EditDefaultsOnly, Category = "Combat")

	float FireRange = 600.f;

	UPROPERTY(EditDefaultsOnly, Category = "Combat")
	float FireRate = 5.f;
	
	FTimerHandle FireRateTimerHandle;

	float GetYawOffsetToFaceTarget(const UStaticMeshComponent* TowerPtr, const ATank* TankPtr);
	void CheckFireCondition();
};


