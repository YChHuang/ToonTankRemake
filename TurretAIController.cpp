// Fill out your copyright notice in the Description page of Project Settings.


#include "TurretAIController.h"
#include "Tower.h"
#include "Kismet/GameplayStatics.h"
#include "Tank.h"
#include "Kismet/KismetMathLibrary.h"
#include "Engine/TimerHandle.h"

// TurretAIController.cpp
void ATurretAIController::OnPossess(APawn* InPawn)
{
    Super::OnPossess(InPawn);

    GetWorldTimerManager().SetTimer(FireRateTimerHandle, this, &ATurretAIController::CheckFireCondition, FireRate, true);
    Tower = Cast<ATower>(InPawn);
	APawn* PlayerPawn = UGameplayStatics::GetPlayerPawn(GetWorld(), 0);
	Tank = Cast<ATank>(PlayerPawn);

    UE_LOG(LogTemp, Warning, TEXT("Hello pawn"))

}

void ATurretAIController::Tick(float DeltaSeconds)
{
    Super::Tick(DeltaSeconds);

    PrimaryActorTick.bCanEverTick = true;
	if (InFireRange() && Tank && Tower && Tower->GetTurret())
	{
        
        float YawOffset = GetYawOffsetToFaceTarget(Tower->GetTurret(), Tank);
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

            Tower->RotateTurret(Step);
        }

	}
}

bool ATurretAIController::InFireRange()
{
	if (Tank && Tower)
	{
		float Distance = FVector::Dist(Tower->GetActorLocation(), Tank->GetActorLocation());
		// Check to see if the tank is in range
		if (Distance < FireRange)
		{
			// if in rnage, rotate turret toward tank
			return true;
		}

	}
	return false;
}

float ATurretAIController::GetYawOffsetToFaceTarget(const UStaticMeshComponent* TowerPtr, const ATank* TankPtr)
{
    if (!TowerPtr || !TankPtr)
    {
        return 0.f; // �C��׋�C�Έڗʈ� 0
    }


    // �擾�_�҈ʒu
    FVector SourceLocation = TowerPtr->GetComponentLocation();
    FVector TargetLocation = TankPtr->GetActorLocation();

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
	if (!Tank) 
	{
		return;
	}
	if (InFireRange() && Tank->bAlive && Tower) 
	{
		Tower->Fire();
	}
}