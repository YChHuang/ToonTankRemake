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
#include "AIController.h"



ATank::ATank()
{
	PrimaryActorTick.bCanEverTick = true;

	SpringArmComp = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArm"));
	SpringArmComp->SetupAttachment(RootComponent);
	SpringArmComp->bUsePawnControlRotation = false; 
	SpringArmComp->bInheritPitch = false;
	SpringArmComp->bInheritYaw = false;
	SpringArmComp->bInheritRoll = false;
	SpringArmComp->SetUsingAbsoluteRotation(true);

	CameraComp = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera"));
	CameraComp->SetupAttachment(SpringArmComp);

	MovementComponent = CreateDefaultSubobject<UTankPawnMovementComponent>(TEXT("MovementComponent"));
	MovementComponent->UpdatedComponent = RootComponent;

	AbilitySystemComponent = CreateDefaultSubobject<UAbilitySystemComponent>(TEXT("AbilitySystemComponent"));
	AbilitySystemComponent->SetIsReplicated(true);
	AbilitySystemComponent->SetReplicationMode(EGameplayEffectReplicationMode::Mixed);

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




void ATank::BeginPlay()
{
	Super::BeginPlay();

	TankPlayerController = Cast<APlayerController>(GetController());
	
	
	if (TankPlayerController)
	{
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
	else if (AutoControllerClass)
	{
		//UE_LOG(LogTemp, Display, TEXT("Get controller"));
		AAIController* AIController = GetWorld()->SpawnActor<AAIController>(AutoControllerClass);
		if (AIController)
		{
			AIController->Possess(this);
		}
	}
	
}

void ATank::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	RotateSpringArm();
}



void ATank::PossessedBy(AController* NewController)
{
	Super::PossessedBy(NewController);
	//這方法只要複寫，不須呼叫，之前做錯了

	if (HasAuthority() && AbilitySystemComponent)
	{
		AbilitySystemComponent->InitAbilityActorInfo(this, this);

		AbilitySystemComponent->GiveAbility(
		FGameplayAbilitySpec(LaserFireAbilityClass, 1, INDEX_NONE, this)
		);

		UE_LOG(LogTemp, Warning, TEXT("LaserFire ability granted!"));

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

	float DeltaYaw = FMath::FindDeltaAngleDegrees(CurrentRotation.Yaw, TargetRotation.Yaw);

	float NewYaw = CurrentRotation.Yaw + FMath::FInterpTo(0.f, DeltaYaw, GetWorld()->GetDeltaSeconds(), 10.f);
	CurrentRotation.Yaw = NewYaw;
	SpringArmComp->SetRelativeRotation(CurrentRotation);
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
			float CurrentAmmo = AttrSet->GetAmmo();
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
		FVector Forward = BaseMesh->GetForwardVector();
		MovementComponent->AddInputVector(Forward * InputValue);
	}

}


void ATank::Turn(const FInputActionValue& inValue)
{
	//TODO:將這個邏輯丟給movementComponent
	float InputValue = inValue.Get<float>();
	FRotator DeltaRotation = FRotator::ZeroRotator;
	DeltaRotation.Yaw = InputValue * TurnRate * UGameplayStatics::GetWorldDeltaSeconds(this);
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



