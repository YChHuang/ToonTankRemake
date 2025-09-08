// Fill out your copyright notice in the Description page of Project Settings.


#include "EnemySpawner.h"
#include "Tower.h"
#include "ToonTanksGameModeBase.h"
#include "Kismet/GameplayStatics.h"
#include "Engine/HitResult.h"
#include "SpawnManager.h"
#include "BasePawn.h"
#include "Components/CapsuleComponent.h"


// Sets default values
AEnemySpawner::AEnemySpawner()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

void AEnemySpawner::DisableSpawn()
{
    //Disable the timer which continue spawn actor
    //if (GetWorld()->GetTimerManager().IsTimerActive(SpawnTimer))
    //{
    //    UE_LOG(LogTemp, Warning, TEXT("DisableSpawning"));
    //    GetWorld()->GetTimerManager().ClearTimer(SpawnTimer);
    //}
    //else
    //{
    //    UE_LOG(LogTemp, Warning, TEXT("SpawnTimer not active"));
    //}
    
}



// Called when the game starts or when spawned
void AEnemySpawner::BeginPlay()
{
	Super::BeginPlay();

    GameModeBase = Cast<AToonTanksGameModeBase>(UGameplayStatics::GetGameMode(this));
    Manager = Cast<ASpawnManager>(
        UGameplayStatics::GetActorOfClass(GetWorld(), ASpawnManager::StaticClass()));
    if (Manager)
    {
        Manager->RegisterSpawner(this);
    }
}

// Called every frame
void AEnemySpawner::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}


ABasePawn* AEnemySpawner::SpawnEnemy()
{
    //spawn tower
    if (!EnemyClass) return nullptr;
    
    FVector SpawnLocation = GetSpawnLocation();
    SpawnLocation.Z += GetCapsuleHeightOffset(EnemyClass);
    
    FRotator SpawnRotation = FRotator::ZeroRotator;
    ABasePawn* SpawnedEnemy = GetWorld()->SpawnActor<ABasePawn>(EnemyClass, SpawnLocation, SpawnRotation);
    return SpawnedEnemy;
}



FVector AEnemySpawner::GetSpawnLocation()
{
    FVector Origin = GetActorLocation();
    FVector RandomOffset = FVector(FMath::FRandRange(-500.f, 500.f), FMath::FRandRange(-500.f, 500.f), 0.f);
    FVector TraceStart = Origin + RandomOffset + FVector(0.f, 0.f, 0);
    FVector TraceEnd = Origin + RandomOffset - FVector(0.f, 0.f, 0 + 500);
    FHitResult Hit;

    if (GetWorld()->LineTraceSingleByChannel(Hit, TraceStart, TraceEnd, ECC_Visibility))
    {
        return Hit.Location;
    }
    return Origin + RandomOffset;
}

float AEnemySpawner::GetCapsuleHeightOffset(TSubclassOf<ABasePawn> Class)
{
    FTransform SpawnTransform;
    ABasePawn* TempActor = GetWorld()->SpawnActorDeferred<ABasePawn>(Class, SpawnTransform);

    float HeightOffset = 1;
    if (TempActor)
    {
        UGameplayStatics::FinishSpawningActor(TempActor, SpawnTransform);
        //UCapsuleComponent* GetCapsule()
        const UCapsuleComponent* capsul = TempActor->GetCapsule();
        float capsuleHeight = 0;
        if (capsul)
        {
            capsuleHeight = capsul->GetScaledCapsuleHalfHeight();
        }
        HeightOffset += capsuleHeight;
        TempActor->Destroy();
    }
    return HeightOffset;
}

