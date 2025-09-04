// Fill out your copyright notice in the Description page of Project Settings.

#include "BasePawn.h"
#include "Components/CapsuleComponent.h"
#include "Kismet/GameplayStatics.h"
#include "FireProjectTile.h"
#include "Particles/ParticleSystemComponent.h"


// Sets default values
ABasePawn::ABasePawn()
{
 	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	CapsuleComp = CreateDefaultSubobject<UCapsuleComponent>(TEXT("Capsule Collider"));
	RootComponent = CapsuleComp;

	BaseMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Base Mesh"));
	BaseMesh->SetupAttachment(CapsuleComp);

	TurretMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Turrent Mesh"));
	TurretMesh->SetupAttachment(CapsuleComp);

	ProjectileSpawnPoint = CreateDefaultSubobject<USceneComponent>(TEXT("Projectile SpawnPoint"));
	ProjectileSpawnPoint->SetupAttachment(TurretMesh);
}


void ABasePawn::HandleDestruction()
{

	if (DeathParticles) 
	{
		UGameplayStatics::SpawnEmitterAtLocation(this, DeathParticles, GetActorLocation(), GetActorRotation());
	}

	if (DeathSound)
	{
		UGameplayStatics::PlaySoundAtLocation(this, DeathSound, GetActorLocation());
	}

	if (DeathCameraShakeClass) {
		GetWorld()->GetFirstPlayerController()->ClientStartCameraShake(DeathCameraShakeClass);
	}
}

const UCapsuleComponent* ABasePawn::GetCapsule() const
{
	//Need capsule to get half height when spawning a tower

	if (!CapsuleComp) return nullptr;
	return CapsuleComp;
}



void ABasePawn::RotateTurret(const FInputActionValue& Value)
{
	float LookInput = Value.Get<float>();
	float Sensitivity = 100.f; // degrees per second

	float DeltaYaw = LookInput * Sensitivity * UGameplayStatics::GetWorldDeltaSeconds(this);

	FRotator CurrentRotation = TurretMesh->GetComponentRotation();
	CurrentRotation.Yaw += DeltaYaw;

	TurretMesh->SetWorldRotation(CurrentRotation);

}

void ABasePawn::Fire()
{

	AFireProjectTile* Projectile = GetWorld()->SpawnActor<AFireProjectTile>(
		ProjecttileClass,
		ProjectileSpawnPoint->GetComponentLocation(),
		ProjectileSpawnPoint->GetComponentRotation()
	);
	Projectile->SetOwner(this);
}





