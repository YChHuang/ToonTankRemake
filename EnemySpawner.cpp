// Fill out your copyright notice in the Description page of Project Settings.


#include "EnemySpawner.h"
#include "Kismet/GameplayStatics.h"
#include "Engine/HitResult.h"
#include "BasePawn.h"
#include "Engine/TimerHandle.h"
#include "GameFramework/Actor.h"

// Sets default values
AEnemySpawner::AEnemySpawner()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}



// Called when the game starts or when spawned
void AEnemySpawner::BeginPlay()
{
	Super::BeginPlay();


}

// Called every frame
void AEnemySpawner::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}


void AEnemySpawner::SpawnEnemy()
{
    // === 無可生成敵人，提早返回 ===
    if (!EnemyClass) return;
    
    // === 隨機生成與生成在地上 === 

    FVector SpawnLocation = GetSpawnLocation();
    //towerheight is 80
    float OffsetZ = 0;

    // 暫時把測試過的hardcode存在set內
    if (float* FoundOffset = EnemyHeightOffsets.Find(EnemyClass))
    {
        OffsetZ = *FoundOffset;
        UE_LOG(LogTemp, Warning, TEXT("OffsetZ is %f"), OffsetZ);
    }


    SpawnLocation.Z += OffsetZ;

    // 嘗試生成敵人
    FRotator SpawnRotation = FRotator::ZeroRotator;
    SpawnedEnemy = GetWorld()->SpawnActor<ABasePawn>(EnemyClass, SpawnLocation, SpawnRotation);

    // 檢測生成是否成功
    if (SpawnedEnemy)
    {
        //標記Enemy用來區分實例
        SpawnedEnemy->Tags.Add("Enemy");
        UE_LOG(LogTemp, Warning, TEXT("Enemy Spawned"));
        //通知spawn manager
        OnEnemySpawned.Broadcast(SpawnedEnemy);
    }

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

