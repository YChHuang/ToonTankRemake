// Fill out your copyright notice in the Description page of Project Settings.


#include "SpawnManager.h"
#include "EnemySpawner.h"
#include "BasePawn.h"
#include "ToonTanksGameModeBase.h"

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

    GameMode = Cast<AToonTanksGameModeBase>(GetWorld()->GetAuthGameMode());

    if (GameMode)
    {
        UE_LOG(LogTemp, Warning, TEXT("Manager get gamemode!!"));
    }

	
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



void ASpawnManager::StartWave(int WaveIndex)
{
    CurrentWave = WaveIndex;
    //TODO:可以讓變數修改
    RemainingSpawns = 5;
    UE_LOG(LogTemp, Warning, TEXT("Wave %d ganna spawn %d enemys!!"), CurrentWave , RemainingSpawns)
 
    GetWorldTimerManager().SetTimer(
        SpawnTimerHandle,
        this,
        &ASpawnManager::SpawnTick,
        SpawnInterval,
        true
    );

}

void ASpawnManager::SpawnTick()
{
    if (RemainingSpawns > 0)
    {
        // 這裡可以選擇哪個 Spawner 要生怪（隨機 / 輪流）
        //RequestSpawn(ChooseSpawner());

        if (SpawnerList.Num() > 0)
        {
            AEnemySpawner* Spawner = SpawnerList[0];
            if (Spawner)
            {
                ABasePawn* basePawn = Spawner->SpawnEnemy();
                if (GameMode && basePawn)
                {
                    GameMode->addTower();
                    RemainingSpawns--;
                    UE_LOG(LogTemp, Warning, TEXT("Remian %d enemy will be spawn"), RemainingSpawns);
                }
            }
        }
        
    }
    else
    {
        // 生完了就停 Timer
        GetWorldTimerManager().ClearTimer(SpawnTimerHandle);
    }
}


void ASpawnManager::OnEnemySpawned()
{
    AliveEnemyCount++;
}