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
#include "ABS_Tank.h"
#include "GA_LaserFire.h"



ATank::ATank()
{
	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	//@TODO: Fix Rotation while pitch is changing

	SpringArmComp = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArm"));
	SpringArmComp->SetupAttachment(RootComponent);
	SpringArmComp->bUsePawnControlRotation = false; // 確保不被 Pawn 控制
	SpringArmComp->bInheritPitch = false;
	SpringArmComp->bInheritYaw = false;
	SpringArmComp->bInheritRoll = false;
	SpringArmComp->SetUsingAbsoluteRotation(true);

	CameraComp = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera"));
	CameraComp->SetupAttachment(SpringArmComp);

	MovementComponent = CreateDefaultSubobject<UTankPawnMovementComponent>(TEXT("MovementComponent"));
	MovementComponent->UpdatedComponent = RootComponent;

	AbilitySystemComponent = CreateDefaultSubobject<UAbilitySystemComponent>(TEXT("AbilitySystemComponent"));
	ABS_Tank = CreateDefaultSubobject<UABS_Tank>(TEXT("AttributeSet"));

}



void ATank::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);


	UEnhancedInputComponent* EnhancedInputComponent = CastChecked<UEnhancedInputComponent>(PlayerInputComponent);

	if (EnhancedInputComponent) 
	{
		if (MoveAction)
		{
			EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, this, &ATank::Move);
		}
		if (TurnTankAction)
		{
			EnhancedInputComponent->BindAction(TurnTankAction, ETriggerEvent::Triggered, this, &ATank::Turn);
		}
		if (FireAction)
		{
			EnhancedInputComponent->BindAction(FireAction, ETriggerEvent::Started, this, &ATank::GAS_fire);
		}

		if (RotateTurretAction)
		{
			EnhancedInputComponent->BindAction(RotateTurretAction, ETriggerEvent::Triggered, this, &ATank::RotateTurret);
		}
	}
}

void ATank::RotateSpringArm()
{
	if (!TurretMesh || !SpringArmComp)
	{
		return;
	}

	FRotator CurrentRotation = SpringArmComp->GetComponentRotation();
	FRotator TargetRotation = TurretMesh->GetComponentRotation();

	// 計算角度差，考慮循環
	float DeltaYaw = FMath::FindDeltaAngleDegrees(CurrentRotation.Yaw, TargetRotation.Yaw);

	// 插值 Yaw（考慮最短路徑）
	float NewYaw = CurrentRotation.Yaw + FMath::FInterpTo(0.f, DeltaYaw, GetWorld()->GetDeltaSeconds(), 10.f);
	CurrentRotation.Yaw = NewYaw;
	SpringArmComp->SetRelativeRotation(CurrentRotation);
}

void ATank::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);


	RotateSpringArm();



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


void ATank::BeginPlay()
{
	Super::BeginPlay();

	TankPlayerController = Cast<APlayerController>(GetController());
	
	
	if (TankPlayerController)
	{
		// GAS call ASC
		PossessedBy(TankPlayerController);

		if (AbilitySystemComponent && HasAuthority())
		{
			AbilitySystemComponent->GiveAbility(
				FGameplayAbilitySpec(LaserFireAbilityClass, 1, 0)
			);



		}

		//EIS
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

void ATank::PossessedBy(AController* NewController)
{
	Super::PossessedBy(NewController);

	if (HasAuthority() && AbilitySystemComponent)
	{
		AbilitySystemComponent->InitAbilityActorInfo(this, this);

		FGameplayAbilitySpec LaserFireSpec(
			UGA_LaserFire::StaticClass(), // 技能類型
			1,                             // 技能等級
			INDEX_NONE,                    // InputID (先不綁定輸入)
			this                           // SourceObject (誰給的這個技能)
		);




		AbilitySystemComponent->GiveAbility(LaserFireSpec);

		UE_LOG(LogTemp, Warning, TEXT("LaserFire ability granted!"));

	}
}

void ATank::GAS_fire(const FInputActionValue& inValue)
{
	if (!AbilitySystemComponent)
	{
		UE_LOG(LogTemp, Warning, TEXT("Pawn Can't Find ASC"));
		return;
	}


	if (LaserFireAbilityClass)
	{
		AbilitySystemComponent->TryActivateAbilityByClass(LaserFireAbilityClass);
		const UABS_Tank* AttrSet = AbilitySystemComponent->GetSet<UABS_Tank>();
		if (AttrSet)
		{
			float CurrentAmmo = AttrSet->GetAmmo(); // 這裡用 ATTRIBUTE_ACCESSORS 產生的 Getter
			UE_LOG(LogTemp, Warning, TEXT("Current Ammo: %f"), CurrentAmmo);
		}
	}

}


void ATank::Move(const FInputActionValue& inValue)
{
	if (!BaseMesh || !MovementComponent)
	{
		return;
	}
	float InputValue = inValue.Get<float>();
	

	if (MovementComponent && InputValue != 0.f)
	{
		/*UE_LOG(LogTemp, Warning, TEXT("Move input : %f"), InputValue);*/
		FVector Forward = BaseMesh->GetForwardVector();
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
	DeltaRotation.Yaw = InputValue * TurnRate * UGameplayStatics::GetWorldDeltaSeconds(this);
	//BaseMesh->AddLocalRotation(DeltaRotation);
	AddActorLocalRotation(DeltaRotation);

}

bool ATank::GetAimingPoint(FVector& OutPoint) const
{

	if (bHasGamepadInput) 
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

	return false;
}



