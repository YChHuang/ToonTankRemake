// Fill out your copyright notice in the Description page of Project Settings.


#include "TankPawnMovementComponent.h"

void UTankPawnMovementComponent::TickComponent(float DeltaTime, enum ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
    Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

    //____Move forward_____

    if (!PawnOwner || !UpdatedComponent || ShouldSkipUpdate(DeltaTime))
        return;

    FVector DesiredMove = ConsumeInputVector().GetClampedToMaxSize(1.f) * MoveSpeed * DeltaTime;
    if (!DesiredMove.IsNearlyZero())
    {
        FHitResult Hit;
        SafeMoveUpdatedComponent(DesiredMove, UpdatedComponent->GetComponentRotation(), true, Hit);

        if (Hit.IsValidBlockingHit())
        {
            SlideAlongSurface(DesiredMove, 1.f - Hit.Time, Hit.Normal, Hit);
        }
    }

    // Gravity constant
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