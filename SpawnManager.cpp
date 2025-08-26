// Fill out your copyright notice in the Description page of Project Settings.


#include "SpawnManager.h"
#include "EnemySpawner.h"

// Sets default values
ASpawnManager::ASpawnManager()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void ASpawnManager::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ASpawnManager::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void ASpawnManager::RegisterSpawner(AEnemySpawner* Spawner)
{
    if (Spawner && !SpawnerList.Contains(Spawner))
    {
        SpawnerList.Add(Spawner);
    }
}

void ASpawnManager::SetAllSpawnersEnabled(bool bEnable)
{
    for (auto* Spawner : SpawnerList)//notice that can be spawn
    {
        if (Spawner)
        {
            Spawner->SetSpawnEnable(bEnable);
        }
    }
}

void ASpawnManager::StartWave(int WaveIndex)
{
    CurrentWave = WaveIndex;
    UE_LOG(LogTemp, Warning, TEXT("Wave %d"), CurrentWave)
    SetAllSpawnersEnabled(true);
}

void ASpawnManager::StopAllSpawns()
{
    SetAllSpawnersEnabled(false);
}

void ASpawnManager::OnEnemySpawned()
{
    AliveEnemyCount++;
}