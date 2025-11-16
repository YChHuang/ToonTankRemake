// Fill out your copyright notice in the Description page of Project Settings.


#include "TurretAIController.h"
#include "Kismet/GameplayStatics.h"
#include "BasePawn.h"
#include "Kismet/KismetMathLibrary.h"
#include "Engine/TimerHandle.h"
#include "Tank.h"
#include "NavigationSystem.h"
#include "Navigation/PathFollowingComponent.h"
#include "TankPawnMovementComponent.h"

void ATurretAIController::OnPossess(APawn* InPawn)
{
    Super::OnPossess(InPawn);
    PrimaryActorTick.bCanEverTick = true;
    GetWorldTimerManager().SetTimer(FireRateTimerHandle, this, &ATurretAIController::CheckFireCondition, FireRate, true);
    Target = Cast<ABasePawn>(InPawn);
	Player = Cast<ABasePawn>(UGameplayStatics::GetPlayerPawn(GetWorld(), 0));
    UE_LOG(LogTemp, Warning, TEXT("Target name is %s"), *Target->GetName());
    UE_LOG(LogTemp, Warning, TEXT("Player name is %s"), *Player->GetName());
}

void ATurretAIController::Tick(float DeltaSeconds)
{
    Super::Tick(DeltaSeconds);

    
	if (InFireRange() && Player && Target && Target->GetTurret())
	{
        
        float YawOffset = GetYawOffsetToFaceTarget(Target->GetTurret(), Player);

        if (FMath::Abs(YawOffset) > AimTolerance)
        {
            float MaxStep = RotationSpeed * DeltaSeconds;
            float Step = FMath::Clamp(YawOffset, -MaxStep, MaxStep);

            Target->RotateTurret(Step);
        }

	}

    if (ATank* Tank = Cast<ATank>(Target))
    {
       /* UE_LOG(LogTemp, Warning, TEXT("Tank say hello from controller"))*/
        RotateTowardsPlayer();
        MoveTowardsPlayer();
    }
}

bool ATurretAIController::InFireRange()
{
	if (Player && Target)
	{
		float Distance = FVector::Dist(Player->GetActorLocation(), Target->GetActorLocation());

		if (Distance < FireRange)
		{

			return true;
		}

	}
	return false;
}

float ATurretAIController::GetYawOffsetToFaceTarget(const UStaticMeshComponent* TowerPtr, const ABasePawn* PlayerPtr)
{
    if (!TowerPtr || !PlayerPtr)
    {
        return 0.f; 
    }


    FVector SourceLocation = TowerPtr->GetComponentLocation();
    FVector TargetLocation = PlayerPtr->GetActorLocation();

    FVector Direction = (TargetLocation - SourceLocation);
    Direction.Z = 0.f;
    Direction.Normalize();

    float CurrentYaw = TowerPtr->GetComponentRotation().Yaw;

    float TargetYaw = Direction.Rotation().Yaw;

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

void ATurretAIController::MoveTowardsPlayer()
{
    // 檢查Player是否存在
    if (!Player)
    {
        UE_LOG(LogTemp, Error, TEXT("Player is NULL!"));
        return;
    }

    // 獲取當前控制的Pawn
    APawn* ControlledPawn = GetPawn();
    if (!ControlledPawn)
    {
        UE_LOG(LogTemp, Error, TEXT("ControlledPawn is NULL!"));
        return;
    }

    // 計算到玩家的距離
    float DistanceToPlayer = FVector::Dist(
        ControlledPawn->GetActorLocation(),
        Player->GetActorLocation()
    );

    // 如果距離超過接受範圍，開始移動
    if (DistanceToPlayer > AcceptanceRadius)
    {
        // 使用UE內建的A*尋路系統
        EPathFollowingRequestResult::Type Result = MoveToActor(
            Player,              // 目標Actor
            AcceptanceRadius,    // 停止距離
            true,                // 停在目標前
            true,                // 使用尋路
            false,               // 不投影到導航網格
            nullptr,             // 過濾類別
            true                 // 允許部分路徑
        );

        // Debug：顯示移動結果
        switch (Result)
        {
        case EPathFollowingRequestResult::RequestSuccessful:
            UE_LOG(LogTemp, Display, TEXT("Moving to Player (Distance: %.0f)"), DistanceToPlayer);
            break;
        case EPathFollowingRequestResult::Failed:
            UE_LOG(LogTemp, Warning, TEXT("MoveToActor Failed! Check NavMesh"));
            break;
        case EPathFollowingRequestResult::AlreadyAtGoal:
            UE_LOG(LogTemp, Display, TEXT("Already at goal"));
            break;
        }
    }
    else
    {
        // 已經在射程內，停止移動
        StopMovement();
        UE_LOG(LogTemp, Display, TEXT("In range, stopped (Distance: %.0f)"), DistanceToPlayer);
    }
}


void ATurretAIController::RotateTowardsPlayer()
{
    if (!Player) return;

    APawn* ControlledPawn = GetPawn();
    if (!ControlledPawn) return;

    // 計算朝向玩家的方向
    FVector DirectionToPlayer = (Player->GetActorLocation() - ControlledPawn->GetActorLocation()).GetSafeNormal();

    // 轉換為旋轉角度（只旋轉Yaw，保持水平）
    FRotator TargetRotation = DirectionToPlayer.Rotation();
    TargetRotation.Pitch = 0.0f;
    TargetRotation.Roll = 0.0f;

    // 平滑插值旋轉
    FRotator CurrentRotation = ControlledPawn->GetActorRotation();
    FRotator NewRotation = FMath::RInterpTo(
        CurrentRotation,
        TargetRotation,
        GetWorld()->GetDeltaSeconds(),
        5.0f  // 旋轉速度，可以調整
    );

    // 應用旋轉
    ControlledPawn->SetActorRotation(NewRotation);
}