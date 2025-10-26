// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PawnMovementComponent.h"
#include "TankPawnMovementComponent.generated.h"

/**
 * 
 */
UCLASS()
class TOONTANKS_API UTankPawnMovementComponent : public UPawnMovementComponent
{
	GENERATED_BODY()


public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement")
	float MoveSpeed = 600.f;

	virtual void TickComponent(float DeltaTime, enum ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
	UPROPERTY(BlueprintReadOnly, Category = "Movement")
	FVector LastGroundNormal = FVector::UpVector;
};
