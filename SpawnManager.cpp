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

	if (WaveIndex > waveConfig.Num())
	{
		UE_LOG(LogTemp, Warning, TEXT("invalid wave"));
		return;
	}
	currentWave = WaveIndex;
	currentEnemyCount = waveConfig[currentWave];
	OnWaveStart.Broadcast(currentEnemyCount);
	

	UE_LOG(LogTemp, Warning, TEXT("Wave %d start!!"), currentWave);
	GetWorldTimerManager().SetTimer(
		WaveTimerHandle,
		this,
		&ASpawnManager::handleWave,
		2.0f,
		true
	);
}

void ASpawnManager::handleWave()
{
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
	currentEnemyCount--;
	if (currentEnemyCount == 0)
	{
		//pause timer
		GetWorldTimerManager().PauseTimer(WaveTimerHandle);
	}

}

// Called when the game starts or when spawned
void ASpawnManager::BeginPlay()
{
	Super::BeginPlay();

	

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
		
		spawner->OnEnemySpawned.AddDynamic(this, &ASpawnManager::handleEnemySpawn);
	}
	
}

// Called every frame
void ASpawnManager::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}
