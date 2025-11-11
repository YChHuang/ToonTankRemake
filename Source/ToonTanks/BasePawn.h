// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "InputAction.h"
#include "BasePawn.generated.h"

UCLASS()
class TOONTANKS_API ABasePawn : public APawn
{
	GENERATED_BODY()

public:
	// Sets default values for this pawn's properties
	ABasePawn();

	void HandleDestruction();

	void HandlePlayerDestruction();

	const class UCapsuleComponent* GetCapsule() const;

	void RotateTurret(const FInputActionValue& Value);

	UStaticMeshComponent* GetTurret();
	UStaticMeshComponent* GetBase();
	USceneComponent* GetProjectileSpawnPoint();
	 

	void Fire();
	bool isAlive();

protected:


	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "ComponentOfCpp", meta = (AllowPrivateAccess = "true"))
	UStaticMeshComponent* TurretMesh;

	float CapsuleHalfHeight = 0;
	bool bAlive = true;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "ComponentOfCpp", meta = (AllowPrivateAccess = "true"))
	UStaticMeshComponent* BaseMesh;

	

private:





	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "ComponentOfCpp", meta = (AllowPrivateAccess = "true"))
	USceneComponent* ProjectileSpawnPoint;

	UPROPERTY(EditDefaultsOnly, Category = "Combat")
	TSubclassOf<class AFireProjectTile> ProjecttileClass;

	UPROPERTY(EditDefaultsOnly, Category = "Combat")
	class UParticleSystem* DeathParticles;

	UPROPERTY(EditAnyWhere, Category = "Combat")
	class USoundBase* DeathSound;

	UPROPERTY(EditAnyWhere, Category = "Combat")
	TSubclassOf<class UCameraShakeBase> DeathCameraShakeClass;


	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "ComponentOfCpp", meta = (AllowPrivateAccess = "true"))
	class UCapsuleComponent* CapsuleComp;
};
