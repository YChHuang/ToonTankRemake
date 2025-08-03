// Fill out your copyright notice in the Description page of Project Settings.


#include "FireProjectTile.h"
#include "GameFramework/ProjectileMovementComponent.h"

// Sets default values
AFireProjectTile::AFireProjectTile()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	ProjectTileMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("ProjectTileMesh"));
	RootComponent = ProjectTileMesh;

	ProjectileMovementComp = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("ProjectTileMoveComp"));
	ProjectileMovementComp->MaxSpeed = 1500.f;
	ProjectileMovementComp->InitialSpeed = 1500.f;


}

// Called when the game starts or when spawned
void AFireProjectTile::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AFireProjectTile::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

