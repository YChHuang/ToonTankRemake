// Fill out your copyright notice in the Description page of Project Settings.


#include "TankPawnMovementComponent.h"

void UTankPawnMovementComponent::TickComponent(float DeltaTime, enum ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
    Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

    //____Move forward_____

    if (!PawnOwner || !UpdatedComponent || ShouldSkipUpdate(DeltaTime))
    {
        UE_LOG(LogTemp, Warning, TEXT("Tick skipped: PawnOwner or UpdatedComponent invalid, or ShouldSkipUpdate is true"));
        return;
    }
        

    FVector DesiredMove = ConsumeInputVector().GetClampedToMaxSize(1.f) * MoveSpeed * DeltaTime;
    if (!DesiredMove.IsNearlyZero())
    {
        FHitResult Hit;
        SafeMoveUpdatedComponent(DesiredMove, UpdatedComponent->GetComponentRotation(), true, Hit);

        UE_LOG(LogTemp, Warning, TEXT("SafeMoveUpdatedComponent done. Hit blocking: %s"), Hit.IsValidBlockingHit() ? TEXT("True") : TEXT("False"));

        if (Hit.IsValidBlockingHit())
        {

            FVector SlopeNormal = Hit.Normal; // 先存下來


            // @TODO hit傳進後會壞掉
            SlideAlongSurface(DesiredMove, 1.f - Hit.Time, Hit.Normal, Hit);


            //// Align rotation to the slope
            //const FVector CurrentForward = UpdatedComponent->GetForwardVector();

            //// Project current forward vector onto the slope plane to keep it tangent to the surface
            //FVector ForwardOnSlope = FVector::VectorPlaneProject(CurrentForward, SlopeNormal).GetSafeNormal();
            //UE_LOG(LogTemp, Warning, TEXT("slope normal vector is : %s"), *ForwardOnSlope.ToString());


            //// Create a rotation with forward aligned to ForwardOnSlope and up aligned to SlopeNormal
            //FRotator TargetRotation = FRotationMatrix::MakeFromXZ(ForwardOnSlope, SlopeNormal).Rotator();

            //// Smooth rotation for better visual
            //FRotator CurrentRotation = UpdatedComponent->GetComponentRotation();
            //float SlopeAlignSpeed = 10.0f; // Adjust for smoothing speed
            //FRotator NewRotation = FMath::RInterpTo(CurrentRotation, TargetRotation, DeltaTime, SlopeAlignSpeed);

            //UE_LOG(LogTemp, Warning, TEXT("NewRotation after interp: %s"), *NewRotation.ToString());
            //UpdatedComponent->SetWorldRotation(NewRotation);
        }
        else
        {
            UE_LOG(LogTemp, Warning, TEXT("No blocking hit detected; skipping slope alignment."));
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

    }



}