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
            // ŒvŽZ”‡›êÅ‘½”\çz‘½­
            float MaxStep = RotationSpeed * DeltaSeconds;

            // ŒÀ§ùçz—ÊC”ð–ÆuŠÔ’µçz
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
        return 0.f; // ”Cˆêˆ×‹óC•ÎˆÚ—Êˆ× 0
    }


    // Žæ“¾™_ŽÒˆÊ’u
    FVector SourceLocation = TowerPtr->GetComponentLocation();
    FVector TargetLocation = PlayerPtr->GetActorLocation();

    // ŒvŽZ–Ú•W•ûŒüiš—ª Z ‚“xj
    FVector Direction = (TargetLocation - SourceLocation);
    Direction.Z = 0.f;
    Direction.Normalize();

    // Žæ“¾ A –Ú‘O“I Yaw
    float CurrentYaw = TowerPtr->GetComponentRotation().Yaw;

    // ŒvŽZ A –ÊŒü B ŠŽù“I–Ú•W Yaw
    float TargetYaw = Direction.Rotation().Yaw;

    // ŒvŽZ•ÎˆÚ—Êi”Íš¡ -180 ~ 180j
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