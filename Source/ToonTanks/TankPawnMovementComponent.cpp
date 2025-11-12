// Fill out your copyright notice in the Description page of Project Settings.


#include "TankPawnMovementComponent.h"
#include "BasePawn.h"

void UTankPawnMovementComponent::BeginPlay()
{
    Super::BeginPlay();
    TankActor = GetOwner();
    if (TankActor)
    {
        MeshComp = TankActor->FindComponentByClass<UStaticMeshComponent>();
    }
}

void UTankPawnMovementComponent::TickComponent(float DeltaTime, enum ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
    Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
    PrimaryComponentTick.bCanEverTick = true;

    if (!PawnOwner || !UpdatedComponent || ShouldSkipUpdate(DeltaTime))
    {
        return;
    }






    FVector DesiredMove = ConsumeInputVector().GetClampedToMaxSize(1.f) * MoveSpeed * DeltaTime;

    if (!DesiredMove.IsNearlyZero())
    {
        FHitResult Hit;
        SafeMoveUpdatedComponent(DesiredMove, UpdatedComponent->GetComponentRotation(), true, Hit);

        if (Hit.IsValidBlockingHit() && MeshComp)
        {
            FVector SlopeNormal = Hit.Normal;
            const float MaxSlopeAngle = 60.0f;
            float SlopeAngle = FMath::RadiansToDegrees(FMath::Acos(FVector::DotProduct(SlopeNormal, FVector::UpVector)));

            if (SlopeAngle <= MaxSlopeAngle)
            {
                SlideAlongSurface(DesiredMove, 1.f - Hit.Time, Hit.Normal, Hit);

                // 關鍵改變：直接在本地空間計算傾斜
                // 1. 獲取相對於 UpdatedComponent 的斜坡法線
                FVector LocalSlopeNormal = UpdatedComponent->GetComponentTransform().InverseTransformVectorNoScale(SlopeNormal);

                UE_LOG(LogTemp, Warning, TEXT("World SlopeNormal: %s"), *SlopeNormal.ToString());
                UE_LOG(LogTemp, Warning, TEXT("Local SlopeNormal: %s"), *LocalSlopeNormal.ToString());

                // 2. 在本地空間構建旋轉
                // 本地 Forward = (1, 0, 0)
                FVector LocalForward = FVector(1, 0, 0);

                // 將 Forward 投影到本地斜坡平面上
                FVector LocalForwardOnSlope = FVector::VectorPlaneProject(LocalForward, LocalSlopeNormal);

                if (LocalForwardOnSlope.SizeSquared() > 0.01f)
                {
                    LocalForwardOnSlope.Normalize();

                    // 3. 構建本地旋轉矩陣（這就是相對旋轉）
                    FRotator TargetRelativeRotation = FRotationMatrix::MakeFromXZ(LocalForwardOnSlope, LocalSlopeNormal).Rotator();

                    UE_LOG(LogTemp, Warning, TEXT("TargetRelativeRotation: P=%f Y=%f R=%f"),
                        TargetRelativeRotation.Pitch, TargetRelativeRotation.Yaw, TargetRelativeRotation.Roll);

                    // 4. 平滑插值
                    FRotator CurrentRelativeRotation = MeshComp->GetRelativeRotation();
                    float SlopeAlignSpeed = 10.0f;
                    FRotator NewRelativeRotation = FMath::RInterpTo(CurrentRelativeRotation, TargetRelativeRotation, DeltaTime, SlopeAlignSpeed);

                    MeshComp->SetRelativeRotation(NewRelativeRotation);
                }
            }
            else
            {
                SlideAlongSurface(DesiredMove, 1.f - Hit.Time, Hit.Normal, Hit);
            }
        }
    }

    //____Gravity_____
    const float Gravity = -980.f;
    Velocity.Z += Gravity * DeltaTime;
    FVector Delta = Velocity * DeltaTime;
    FHitResult Hit;
    SafeMoveUpdatedComponent(Delta, UpdatedComponent->GetComponentRotation(), true, Hit);

    if (Hit.IsValidBlockingHit())
    {
        SlideAlongSurface(Delta, 1.f - Hit.Time, Hit.Normal, Hit);
        Velocity.Z = 0.f;

        // 平地回正 Mesh（相對旋轉）
        if (MeshComp)
        {
            float GroundAngle = FMath::RadiansToDegrees(FMath::Acos(FVector::DotProduct(Hit.Normal, FVector::UpVector)));

            if (GroundAngle < 5.0f)
            {
                FRotator CurrentRelativeRotation = MeshComp->GetRelativeRotation();
                // 目標：相對旋轉回正（Pitch=0, Roll=0, 保持 Yaw）
                FRotator TargetRelativeRotation = FRotator(0.0f, CurrentRelativeRotation.Yaw, 0.0f);

                float ResetSpeed = 5.0f;
                FRotator NewRelativeRotation = FMath::RInterpTo(CurrentRelativeRotation, TargetRelativeRotation, DeltaTime, ResetSpeed);
                MeshComp->SetRelativeRotation(NewRelativeRotation);
            }
        }
    }


}