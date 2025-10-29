// Fill out your copyright notice in the Description page of Project Settings.


#include "SpawnManager.h"
#include "BasePawn.h"
#include "Kismet/GameplayStatics.h"
#include "EnemySpawner.h"




// Sets default values
ASpawnManager::ASpawnManager()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

void ASpawnManager::StartWave(int WaveIndex)
{
	// === 參數驗證 ===
	if (!WaveConfigs.IsValidIndex(WaveIndex))
	{
		UE_LOG(LogTemp, Warning, TEXT("invalid wave"));
		return;
	}


	if (SpawnerList.Num() == 0)
	{
		UE_LOG(LogTemp, Error, TEXT("No spawners found in level! Cannot start wave."));
		return;
	}

	// === 初始化波次狀態 ===
	CurrentWave = WaveIndex;
	CurrentEnemyCount = WaveConfigs[CurrentWave];
	// 通知 GameMode
	OnWaveStart.Broadcast(CurrentEnemyCount);
	
	// === 開始生成計時器 ===
	// 每 SpawnInterval 秒生成一個敵人,直到本波次配額用完
	UE_LOG(LogTemp, Warning, TEXT("Wave %d start!!"), CurrentWave);
	GetWorldTimerManager().SetTimer(
		WaveTimerHandle,
		this,
		&ASpawnManager::handleWave,
		SpawnInterval,
		true
	);
}

void ASpawnManager::handleWave()
{
	//作為計時器的觸發函數
	if (SpawnerList.Num() <= 0)
	{
		UE_LOG(LogTemp, Warning, TEXT("There's no any spawner"))
		return;
	}
	//TODO: choose enemy type by Gm using int:enemytype

	int32 RandomIndex = FMath::RandRange(0, SpawnerList.Num() - 1);
	UE_LOG(LogTemp, Warning, TEXT("There are %d Spawner"), SpawnerList.Num());

	if (SpawnerList[RandomIndex])
	{
		UE_LOG(LogTemp, Warning, TEXT("SpawnEnemy using %d spawner."), RandomIndex);
		SpawnerList[RandomIndex]->SpawnEnemy();
	}
}




void ASpawnManager::handleEnemySpawn(ABasePawn* SpawnedEnemy)
{
	CurrentEnemyCount--;
	if (CurrentEnemyCount == 0)
	{
		//pause timer
		GetWorldTimerManager().PauseTimer(WaveTimerHandle);
	}

}

// Called when the game starts or when spawned
void ASpawnManager::BeginPlay()
{
	Super::BeginPlay();

	
	// === 尋找世界中的spawner ===
	TArray<AActor*> FoundActors;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), AEnemySpawner::StaticClass(), FoundActors);
	for (AActor* Actor : FoundActors)
	{
		Cast<AEnemySpawner>(Actor);
		if (AEnemySpawner* Spawner = Cast<AEnemySpawner>(Actor))
		{
			SpawnerList.Add(Spawner);
		}
	}

	for (AEnemySpawner* spawner : SpawnerList)
	{
		//訂閱spawner
		spawner->OnEnemySpawned.AddDynamic(this, &ASpawnManager::handleEnemySpawn);
	}
	
}

// Called every frame
void ASpawnManager::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}
