// Fill out your copyright notice in the Description page of Project Settings.


#include "TankPawnMovementComponent.h"
#include "BasePawn.h"

void UTankPawnMovementComponent::TickComponent(float DeltaTime, enum ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
    Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

    //____Move forward_____

    if (!PawnOwner || !UpdatedComponent || ShouldSkipUpdate(DeltaTime))
    {
        UE_LOG(LogTemp, Warning, TEXT("Tick skipped: PawnOwner or UpdatedComponent invalid, or ShouldSkipUpdate is true"));
        return;
    }
    TankActor = GetOwner();
    if (TankActor)
    {
        MeshComp = TankActor->FindComponentByClass<UStaticMeshComponent>();
    }

    FRotator StartRotation = UpdatedComponent->GetComponentRotation();
    UE_LOG(LogTemp, Warning, TEXT("=== Tick Start === Rotation: P=%f Y=%f R=%f"),
        StartRotation.Pitch, StartRotation.Yaw, StartRotation.Roll);

    FVector DesiredMove = ConsumeInputVector().GetClampedToMaxSize(1.f) * MoveSpeed * DeltaTime;
    /*UE_LOG(LogTemp, Warning, TEXT("ConsumeInputVector : %s"), *ConsumeInputVector().ToString());*/
    if (!DesiredMove.IsNearlyZero())
    {
        FHitResult Hit;
        

        SafeMoveUpdatedComponent(DesiredMove, UpdatedComponent->GetComponentRotation(), true, Hit);

        UE_LOG(LogTemp, Warning, TEXT("SafeMoveUpdatedComponent done. Hit blocking: %s"), Hit.IsValidBlockingHit() ? TEXT("True") : TEXT("False"));

        if (Hit.IsValidBlockingHit() && MeshComp)
        {

            FVector SlopeNormal = Hit.Normal; // hit傳進SlideAlongSurface後會洗掉一些資訊，先存下normal
            const float MaxSlopeAngle = 60.0f;
            float SlopeAngle = FMath::RadiansToDegrees(FMath::Acos(FVector::DotProduct(SlopeNormal, FVector::UpVector)));

            if (SlopeAngle <= MaxSlopeAngle)
            {
                SlideAlongSurface(DesiredMove, 1.f - Hit.Time, Hit.Normal, Hit);

                // Align rotation to the slope
                const FVector CurrentForward = UpdatedComponent->GetForwardVector();

                FVector ForwardOnSlope = FVector::VectorPlaneProject(CurrentForward, SlopeNormal).GetSafeNormal();

                UE_LOG(LogTemp, Warning, TEXT("CurrentForward: %s"), *CurrentForward.ToString());
                UE_LOG(LogTemp, Warning, TEXT("ForwardOnSlope: %s"), *ForwardOnSlope.ToString());


                if (!ForwardOnSlope.IsNearlyZero(0.01f))
                {
                    FRotator TargetRotation = FRotationMatrix::MakeFromXZ(ForwardOnSlope, SlopeNormal).Rotator();

                    FRotator CurrentRotation = MeshComp->GetComponentRotation();
                    float SlopeAlignSpeed = 10.0f;
                    FRotator NewRotation = FMath::RInterpTo(CurrentRotation, TargetRotation, DeltaTime, SlopeAlignSpeed);

                    MeshComp->SetWorldRotation(NewRotation);
                }
                else
                {
                    // 垂直牆壁，只執行滑動,不調整旋轉
                    SlideAlongSurface(DesiredMove, 1.f - Hit.Time, Hit.Normal, Hit);
                }
 
            }
        }
        
    }




    //____Gravity_____
    const float Gravity = -980.f; // cm/s^2
    // Velocity with vertical gravity applied
    Velocity.Z += Gravity * DeltaTime;
    // Compute movement including vertical velocity
    FVector Delta = Velocity * DeltaTime;
    // Move with sweep to respect ground collisions
    FHitResult Hit;
    SafeMoveUpdatedComponent(Delta, UpdatedComponent->GetComponentRotation(), true, Hit);

    if (Hit.IsValidBlockingHit())
    {
        SlideAlongSurface(Delta, 1.f - Hit.Time, Hit.Normal, Hit);
        Velocity.Z = 0.f; // Reset vertical velocity on ground contact

        // 檢查是否在平地上，如果是就回正 Pitch 和 Roll
        if (MeshComp)
        {
            float GroundAngle = FMath::RadiansToDegrees(FMath::Acos(FVector::DotProduct(Hit.Normal, FVector::UpVector)));

            if (GroundAngle < 5.0f) // 接近平地
            {
                FRotator CurrentRotation = MeshComp->GetComponentRotation();
                // 只回正 Pitch 和 Roll，保持 Yaw 不變
                FRotator TargetRotation = FRotator(0.0f, CurrentRotation.Yaw, 0.0f);

                float ResetSpeed = 1.0f;
                FRotator NewRotation = FMath::RInterpTo(CurrentRotation, TargetRotation, DeltaTime, ResetSpeed);
                MeshComp->SetWorldRotation(NewRotation);

                UE_LOG(LogTemp, Warning, TEXT("Resetting: Current P=%f Y=%f R=%f -> Target P=%f Y=%f R=%f"),
                    CurrentRotation.Pitch, CurrentRotation.Yaw, CurrentRotation.Roll,
                    TargetRotation.Pitch, TargetRotation.Yaw, TargetRotation.Roll);
            }
        }
    }


    FRotator EndRotation = UpdatedComponent->GetComponentRotation();
    UE_LOG(LogTemp, Warning, TEXT("=== Tick End === Rotation: P=%f Y=%f R=%f (Delta Yaw=%f)"),
        EndRotation.Pitch, EndRotation.Yaw, EndRotation.Roll,
        EndRotation.Yaw - StartRotation.Yaw);

}