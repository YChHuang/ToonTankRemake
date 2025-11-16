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
        if (!MeshComp)
        {
            UE_LOG(LogTemp, Error, TEXT("TankPawnMovementComponent: No StaticMeshComponent found!"));
        }
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

    HandleHorizontalMovement(DeltaTime);
    HandleGravityAndVerticalMovement(DeltaTime);


}

void UTankPawnMovementComponent::HandleHorizontalMovement(float DeltaTime)
{
    FVector DesiredMove = ConsumeInputVector().GetClampedToMaxSize(1.f) * MoveSpeed * DeltaTime;

    if (DesiredMove.IsNearlyZero())
    {
        return;
    }

    FHitResult Hit;
    
    SafeMoveUpdatedComponent(DesiredMove, UpdatedComponent->GetComponentRotation(), true, Hit);
    

    if (Hit.IsValidBlockingHit() && MeshComp)
    {
        FVector SlopeNormal = Hit.Normal;
        float SlopeAngle;

        if (IsWalkableSlope(Hit.Normal, SlopeAngle))
        {
            SlideAlongSurface(DesiredMove, 1.f - Hit.Time, Hit.Normal, Hit);
            AlignMeshToSlope(Hit, DeltaTime, SlopeNormal);
        }
        else
        {
            SlideAlongSurface(DesiredMove, 1.f - Hit.Time, Hit.Normal, Hit);
        }
    }
}

void UTankPawnMovementComponent::HandleGravityAndVerticalMovement(float DeltaTime)
{
    const float Gravity = -980.f;
    Velocity.Z += Gravity * DeltaTime;
    FVector Delta = Velocity * DeltaTime;
    FHitResult Hit;
    SafeMoveUpdatedComponent(Delta, UpdatedComponent->GetComponentRotation(), true, Hit);

    if (Hit.IsValidBlockingHit())
    {
        SlideAlongSurface(Delta, 1.f - Hit.Time, Hit.Normal, Hit);
        Velocity.Z = 0.f;

        // 平地回正 Actor 的旋轉
        float GroundAngle = FMath::RadiansToDegrees(FMath::Acos(FVector::DotProduct(Hit.Normal, FVector::UpVector)));

        if (GroundAngle < 5.0f && TankActor)
        {
            FRotator CurrentRotation = TankActor->GetActorRotation();
            FRotator TargetRotation = FRotator(0.0f, CurrentRotation.Yaw, 0.0f);

            float ResetSpeed = 5.0f;
            FRotator NewRotation = FMath::RInterpTo(CurrentRotation, TargetRotation, DeltaTime, ResetSpeed);

            TankActor->SetActorRotation(NewRotation); // ✅ 使用 Set，不是 Add
        }
    }
}

bool UTankPawnMovementComponent::IsWalkableSlope(const FVector& SlopeNormal, float& OutSlopeAngle) const
{
    OutSlopeAngle = FMath::RadiansToDegrees(
        FMath::Acos(FVector::DotProduct(SlopeNormal, FVector::UpVector))
    );

    return OutSlopeAngle <= MaxSlopeAngle;
}

void UTankPawnMovementComponent::AlignMeshToSlope(const FHitResult& Hit, float DeltaTime, FVector SlopeNormal)
{
    

    // 轉換到本地空間
    FVector LocalSlopeNormal = UpdatedComponent->GetComponentTransform().InverseTransformVectorNoScale(SlopeNormal);

    // 本地前進方向
    FVector LocalForward = FVector(1, 0, 0);

    // 投影到斜坡平面
    FVector LocalForwardOnSlope = FVector::VectorPlaneProject(LocalForward, LocalSlopeNormal);

    if (LocalForwardOnSlope.SizeSquared() < 0.01f)
    {
        // 投影失敗，不更新旋轉
        return;
    }

    LocalForwardOnSlope.Normalize();

    // 構建目標旋轉
    FRotator TargetRelativeRotation = FRotationMatrix::MakeFromXZ(LocalForwardOnSlope, LocalSlopeNormal).Rotator();

    // 平滑插值到目標旋轉
    FRotator CurrentRelativeRotation = MeshComp->GetRelativeRotation();
    FRotator NewRelativeRotation = FMath::RInterpTo(
        CurrentRelativeRotation,
        TargetRelativeRotation,
        DeltaTime,
        SlopeAlignSpeed
    );

    
    TankActor->AddActorLocalRotation(NewRelativeRotation);

    /*MeshComp->SetRelativeRotation(NewRelativeRotation);*/
}
