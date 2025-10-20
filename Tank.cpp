// Fill out your copyright notice in the Description page of Project Settings.


#include "Tank.h"
#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"
#include "Kismet/GameplayStatics.h"
#include "DrawDebugHelpers.h"
#include "EnhancedInputSubsystems.h"
#include "InputMappingContext.h"
#include "EnhancedInputComponent.h"
#include "InputAction.h"



ATank::ATank()
{
	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	SpringArmComp = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArm"));
	SpringArmComp->SetupAttachment(RootComponent);

	CameraComp = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera"));
	CameraComp->SetupAttachment(SpringArmComp);

	MovementComponent = CreateDefaultSubobject<UTankPawnMovementComponent>(TEXT("MovementComponent"));
	MovementComponent->UpdatedComponent = RootComponent;

}



void ATank::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);


	UEnhancedInputComponent* EnhancedInputComponent = CastChecked<UEnhancedInputComponent>(PlayerInputComponent);

	if (EnhancedInputComponent) 
	{
		if (MoveAction && MoveTurn)
		{
			EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, this, &ATank::Move);
			EnhancedInputComponent->BindAction(MoveTurn, ETriggerEvent::Triggered, this, &ATank::Turn);
		}

		if (FireAction)
		{
			EnhancedInputComponent->BindAction(FireAction, ETriggerEvent::Started, this, &ATank::Fire);
		}

		if (LookAction)
		{
			EnhancedInputComponent->BindAction(LookAction, ETriggerEvent::Triggered, this, &ATank::RotateTurret);
		}
	}

	//old input from tutorial
	//PlayerInputComponent->BindAxis(TEXT("MoveForward"), this, &ATank::Move);
	//PlayerInputComponent->BindAxis(TEXT("Turn"), this, &ATank::Turn);
	//PlayerInputComponent->BindAction(TEXT("Fire"), IE_Pressed, this, &ATank::Fire);
}

void ATank::RotateSpringArm()
{
	if (TurretMesh && SpringArmComp)
	{
		float CurrentYaw = SpringArmComp->GetRelativeRotation().Yaw;
		float TargetYaw = TurretMesh->GetComponentRotation().Yaw;

		// 計算最短角度差
		float DeltaYaw = FMath::FindDeltaAngleDegrees(CurrentYaw, TargetYaw);

		// 插值速度
		float InterpSpeed = 10.f;

		// 平滑更新Yaw
		float NewYaw = CurrentYaw + DeltaYaw * FMath::Clamp(GetWorld()->GetDeltaSeconds() * InterpSpeed, 0.f, 1.f);

		FRotator NewRotation = SpringArmComp->GetRelativeRotation();
		NewRotation.Yaw = NewYaw * 0.1;
		SpringArmComp->SetRelativeRotation(NewRotation);
	}
}

void ATank::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);


	RotateSpringArm();

	//FVector AimPoint;
	//if (GetAimingPoint(AimPoint))
	//{
	//	RotateTurret(AimPoint);
	//}


	//RotateTurret(AimPoint);
}

void ATank::HandlePlayerDestruction()
{
	Super::HandlePlayerDestruction();
	bAlive = false;
}

void ATank::HandleDestruction()
{
	Super::HandleDestruction();
}


// Called when the game starts or when spawned
void ATank::BeginPlay()
{
	Super::BeginPlay();

	TankPlayerController = Cast<APlayerController>(GetController());

	
	if (TankPlayerController)
	{
		int32 ViewportSizeX, ViewportSizeY;
		TankPlayerController->GetViewportSize(ViewportSizeX, ViewportSizeY);

		ViewportCenterX = ViewportSizeX / 2.0f;
		ViewportCenterY = ViewportSizeY / 2.0f - 250.f;

		UEnhancedInputLocalPlayerSubsystem* Subsystem =
			ULocalPlayer::GetSubsystem< UEnhancedInputLocalPlayerSubsystem>(
				TankPlayerController->GetLocalPlayer()
			);
		if (Subsystem)
		{
			Subsystem->AddMappingContext(TankMappingContext, 0);
		}
	}

	
}


void ATank::Move(const FInputActionValue& inValue)
{
	float InputValue = inValue.Get<float>();


	//FVector DeltaLocation = FVector::ZeroVector;
	//DeltaLocation.X = InputValue * Speed * UGameplayStatics::GetWorldDeltaSeconds(this);
	//AddActorLocalOffset(DeltaLocation, true);

	if (MovementComponent && InputValue != 0.f)
	{
		FVector Forward = GetActorForwardVector();
		MovementComponent->AddInputVector(Forward * InputValue);
	}

}


UPawnMovementComponent* ATank::GetMovementComponent() const
{
	return MovementComponent;
}

void ATank::Turn(const FInputActionValue& inValue)
{
	float InputValue = inValue.Get<float>();
	FRotator DeltaRotation = FRotator::ZeroRotator;
	//Yaw = Value * Deltatime * TurnRate
	DeltaRotation.Yaw = InputValue * TurnRate * UGameplayStatics::GetWorldDeltaSeconds(this);
	AddActorLocalRotation(DeltaRotation, true);
}

bool ATank::GetAimingPoint(FVector& OutPoint) const
{

	if (bHasGamepadInput) // 之後我們會設這個旗標
	{
		OutPoint = CachedGamepadAimPoint;
		return true;
	}
	else if (TankPlayerController)
	{
		FHitResult HitResult;
		TankPlayerController->GetHitResultUnderCursor(
			ECollisionChannel::ECC_Visibility,
			false,
			HitResult
		);
		OutPoint = HitResult.ImpactPoint;
		return true;
	}
	//else if (AIController)
	//{
	//	OutPoint = AIController->GetTargetLocation();
	//	return true;
	//}
	return false;
}

void ATank::OnLook(const FInputActionValue& Value)
{
	FVector2D LookInput = Value.Get<FVector2D>();


	if (!LookInput.IsNearlyZero())
	{
		// 1. 把搖桿方向轉成世界方向
		FVector WorldDirection;
		FVector WorldOrigin;
		TankPlayerController->DeprojectScreenPositionToWorld(
			ViewportCenterX + LookInput.X * AimSensitivity,
			ViewportCenterY - LookInput.Y * AimSensitivity,
			/*Out*/ WorldOrigin,
			/*Out*/ WorldDirection
		);

		// 2. 射線檢測
		FHitResult Hit;
		FVector Start = WorldOrigin;
		FVector End = Start + WorldDirection * 100000.f;
		if (GetWorld()->LineTraceSingleByChannel(Hit, Start, End, ECC_Visibility))
		{
			
			CachedGamepadAimPoint = Hit.ImpactPoint;
			UE_LOG(LogTemp, Warning, TEXT("HasGamePadInput"))
			bHasGamepadInput = true;
		}
	}
}


float ATank::GetPitchFromSlopeNormal(const FVector& Normal, const FVector& Forward)
{
	// Project forward vector on slope plane
	FVector ForwardOnSlope = FVector::VectorPlaneProject(Forward, Normal).GetSafeNormal();

	// Calculate angle between forward and its projection (signed pitch)
	float AngleRad = FMath::Atan2(Normal.Z, FVector::DotProduct(Forward, Normal));
	float PitchDegrees = FMath::RadiansToDegrees(AngleRad);

	// Clamp pitch if needed, e.g. between -45 to 45 degrees
	return FMath::Clamp(PitchDegrees, -45.f, 45.f);
}