// Fill out your copyright notice in the Description page of Project Settings.


#include "TurretAIController.h"
#include "Kismet/GameplayStatics.h"
#include "BasePawn.h"
#include "Kismet/KismetMathLibrary.h"
#include "Engine/TimerHandle.h"

// TurretAIController.cpp
void ATurretAIController::OnPossess(APawn* InPawn)
{
    Super::OnPossess(InPawn);
    PrimaryActorTick.bCanEverTick = true;
    GetWorldTimerManager().SetTimer(FireRateTimerHandle, this, &ATurretAIController::CheckFireCondition, FireRate, true);
    Target = Cast<ABasePawn>(InPawn);
	Player = Cast<ABasePawn>(UGameplayStatics::GetPlayerPawn(GetWorld(), 0));


}

void ATurretAIController::Tick(float DeltaSeconds)
{
    Super::Tick(DeltaSeconds);

    
	if (InFireRange() && Player && Target && Target->GetTurret())
	{
        
        float YawOffset = GetYawOffsetToFaceTarget(Target->GetTurret(), Player);
        //LimitRotation
        const float AimTolerance = 1.f;
        //speed degree/sec
        const float RotationSpeed = 300.f;



        if (FMath::Abs(YawOffset) > AimTolerance)
        {
            // �v�Z����ő��\�z����
            float MaxStep = RotationSpeed * DeltaSeconds;

            // �������z�ʁC��Əu�Ԓ��z
            float Step = FMath::Clamp(YawOffset, -MaxStep, MaxStep);

            Target->RotateTurret(Step);
        }

	}
}

bool ATurretAIController::InFireRange()
{
	if (Player && Target)
	{
		float Distance = FVector::Dist(Player->GetActorLocation(), Target->GetActorLocation());
		// Check to see if the tank is in range
		if (Distance < FireRange)
		{
			// if in rnage, rotate turret toward tank
			return true;
		}

	}
	return false;
}

float ATurretAIController::GetYawOffsetToFaceTarget(const UStaticMeshComponent* TowerPtr, const ABasePawn* PlayerPtr)
{
    if (!TowerPtr || !PlayerPtr)
    {
        return 0.f; // �C��׋�C�Έڗʈ� 0
    }


    // �擾�_�҈ʒu
    FVector SourceLocation = TowerPtr->GetComponentLocation();
    FVector TargetLocation = PlayerPtr->GetActorLocation();

    // �v�Z�ڕW�����i���� Z ���x�j
    FVector Direction = (TargetLocation - SourceLocation);
    Direction.Z = 0.f;
    Direction.Normalize();

    // �擾 A �ڑO�I Yaw
    float CurrentYaw = TowerPtr->GetComponentRotation().Yaw;

    // �v�Z A �ʌ� B �����I�ڕW Yaw
    float TargetYaw = Direction.Rotation().Yaw;

    // �v�Z�Έڗʁi�͚� -180 ~ 180�j
    float YawOffset = FMath::FindDeltaAngleDegrees(CurrentYaw, TargetYaw);

    return YawOffset;
}

void ATurretAIController::CheckFireCondition()
{
	if (!Player)
	{
		return;
	}

	if (InFireRange() && Player->isAlive() && Target)
	{
        Target->Fire();
	}
}