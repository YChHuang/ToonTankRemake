// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PawnMovementComponent.h"
#include "TankPawnMovementComponent.generated.h"

/*TankPawnMovementComponent - 坦克移動掛件
* 職責：
*	對底盤的向量移動
*	爬坡
*	爬坡時轉動roll和pitch以切齊斜坡
*	基礎重力墜落模擬
*/
UCLASS()
class TOONTANKS_API UTankPawnMovementComponent : public UPawnMovementComponent
{
	GENERATED_BODY()

protected:
	virtual void BeginPlay() override;

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement")
	float MoveSpeed = 600.f;

	virtual void TickComponent(float DeltaTime, enum ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
	UPROPERTY(BlueprintReadOnly, Category = "Movement")
	FVector LastGroundNormal = FVector::UpVector;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Slope Alignment")
	float MaxSlopeAngle = 60.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Slope Alignment")
	float SlopeAlignSpeed = 10.0f;

private:
	UStaticMeshComponent* MeshComp;
	AActor* TankActor;

	void HandleHorizontalMovement(float DeltaTime);

	void HandleGravityAndVerticalMovement(float DeltaTime);

	bool IsWalkableSlope(const FVector& SlopeNormal, float& OutSlopeAngle) const;

	void AlignMeshToSlope(const FHitResult& Hit, float DeltaTime, FVector SlopeNormal);
};
