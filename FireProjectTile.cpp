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

	ProjectTileMesh->OnComponentHit.AddDynamic(this, &AFireProjectTile::OnHit);
	
}

// Called every frame
void AFireProjectTile::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void AFireProjectTile::OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
	UE_LOG(LogTemp, Warning, TEXT("HitComponent is %s"), *HitComp->GetName());
	UE_LOG(LogTemp, Warning, TEXT("OtherActor is %s"), *OtherActor->GetName());
	UE_LOG(LogTemp, Warning, TEXT("OtherComp is %s"), *OtherComp->GetName());
}

