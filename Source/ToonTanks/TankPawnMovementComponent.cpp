// Fill out your copyright notice in the Description page of Project Settings.


#include "TankPawnMovementComponent.h"
#include "BasePawn.h"


UTankPawnMovementComponent::UTankPawnMovementComponent()
{
    PrimaryComponentTick.bCanEverTick = true;
}

void UTankPawnMovementComponent::BeginPlay()
{
    Super::BeginPlay();
    TankActor = GetOwner();
    PawnOwner = Cast<APawn>(GetOwner());
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

    /*UE_LOG(LogTemp, Warning, TEXT("MoveForwardVector is : %s"), *DesiredMove.ToString());*/

    

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

        float GroundAngle = FMath::RadiansToDegrees(FMath::Acos(FVector::DotProduct(Hit.Normal, FVector::UpVector)));

        if (GroundAngle < 5.0f && TankActor)
        {
            FRotator CurrentRotation = TankActor->GetActorRotation();
            FRotator TargetRotation = FRotator(0.0f, CurrentRotation.Yaw, 0.0f);

            float ResetSpeed = 5.0f;
            FRotator NewRotation = FMath::RInterpTo(CurrentRotation, TargetRotation, DeltaTime, ResetSpeed);

            TankActor->SetActorRotation(NewRotation);
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
    
    FVector LocalSlopeNormal = UpdatedComponent->GetComponentTransform().InverseTransformVectorNoScale(SlopeNormal);

    FVector LocalForward = FVector(1, 0, 0);
    FVector LocalForwardOnSlope = FVector::VectorPlaneProject(LocalForward, LocalSlopeNormal);

    if (LocalForwardOnSlope.SizeSquared() < 0.01f)
    {
        return;
    }

    LocalForwardOnSlope.Normalize();

    FRotator TargetRelativeRotation = FRotationMatrix::MakeFromXZ(LocalForwardOnSlope, LocalSlopeNormal).Rotator();

    FRotator CurrentRelativeRotation = MeshComp->GetRelativeRotation();
    FRotator NewRelativeRotation = FMath::RInterpTo(
        CurrentRelativeRotation,
        TargetRelativeRotation,
        DeltaTime,
        SlopeAlignSpeed
    );

    
    TankActor->AddActorLocalRotation(NewRelativeRotation);

}

void UTankPawnMovementComponent::AddInputVector(FVector WorldVector, bool bForce)
{
    /*UE_LOG(LogTemp, Warning, TEXT("MoveForwardVector is : %s"), *WorldVector.ToString());*/
    PendingInputVector += WorldVector;
}


void UTankPawnMovementComponent::RequestDirectMove(const FVector& MoveVelocity, bool bForceMaxSpeed)
{

    if (!MoveVelocity.IsZero())
    {
        FVector Direction = MoveVelocity.GetSafeNormal();
        AddInputVector(Direction, true);
    }
}

void UTankPawnMovementComponent::RequestPathMove(const FVector& MoveInput)
{
    if (!MoveInput.IsZero())
    {
        AddInputVector(MoveInput, true);
    }
}

FVector UTankPawnMovementComponent::ConsumeInputVector()
{
    FVector Result = PendingInputVector;
    PendingInputVector = FVector::ZeroVector;
    /*UE_LOG(LogTemp, Warning, TEXT("MoveForwardVector is : %s"), *Result.ToString());*/
    return Result;
}