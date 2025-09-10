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

void ASpawnManager::handleEnemySpawn(ABasePawn* SpawnedEnemy)
{
	UE_LOG(LogTemp, Warning, TEXT("Manager received broadcast"));
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
		UE_LOG(LogTemp, Warning, TEXT("GM got Spawner"));
		spawner->OnEnemySpawned.AddDynamic(this, &ASpawnManager::handleEnemySpawn);
	}
}

// Called every frame
void ASpawnManager::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}
