// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BasePawn.h"
#include "InputAction.h"
#include "TankPawnMovementComponent.h"
#include "Components/StaticMeshComponent.h"
#include "AbilitySystemComponent.h"
#include "AbilitySystemInterface.h"
#include "Tank.generated.h"


/**
 * 得理解繼承多個的意思
 *
 */
UCLASS()
class TOONTANKS_API ATank : public ABasePawn, public IAbilitySystemInterface
{
	GENERATED_BODY()
	
public:
	ATank();

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	//GAS官方文件的設置初始化
	virtual UAbilitySystemComponent* GetAbilitySystemComponent() const override
	{
		return AbilitySystemComponent;
	}


	// Called every frame
	virtual void Tick(float DeltaTime) override;

	void HandlePlayerDestruction();

	void HandleDestruction();

	APlayerController* GetTankPlayerController() const { return TankPlayerController; }

	


protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input)
	class UInputMappingContext* TankMappingContext;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input)
	class UInputAction* MoveAction;


	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input)
	class UInputAction* RotateTurretAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input)
	UInputAction* FireAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input)
	UInputAction* TurnTankAction;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Movement")
	UTankPawnMovementComponent* MovementComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "GAS")
	UAbilitySystemComponent* AbilitySystemComponent;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "GAS")
	class UABS_Tank* ABS_Tank;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Camera")
	USceneComponent* CameraPivot;

	virtual UPawnMovementComponent* GetMovementComponent() const override;

private:
	UPROPERTY(VisibleAnywhere, Category = "Components")
	class USpringArmComponent* SpringArmComp;

	UPROPERTY(VisibleAnywhere, Category = "Components")
	class UCameraComponent* CameraComp;

	void RotateSpringArm();


	UPROPERTY(EditDefaultsOnly, Category = "Abilities")
	TSubclassOf<UGameplayAbility> LaserFireAbilityClass;

	void Move(const FInputActionValue& inValue);

	void Turn(const FInputActionValue& inValue);

	bool GetAimingPoint(FVector& OutPoint) const;

	void OnLook(const FInputActionValue& Value);

	void PossessedBy(AController* NewController);

	void GAS_fire(const FInputActionValue& inValue);


	float ViewportCenterX = 0.f;

	float ViewportCenterY = 0.f;

	UPROPERTY(EditAnywhere, Category = "Input")
	float AimSensitivity = 1000.f;
	
	bool bHasGamepadInput = false;

	FVector CachedGamepadAimPoint;


	UPROPERTY(EditAnywhere, Category = "Movement")
	float Speed = 400.f;

	UPROPERTY(EditAnywhere, Category = "Movement")
	float TurnRate = 50.f;

	APlayerController* TankPlayerController;
};
