// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "SpawnManager.generated.h"

UCLASS()
class TOONTANKS_API ASpawnManager : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ASpawnManager();
	void RegisterSpawner(class AEnemySpawner* Spawner);
	void SetAllSpawnersEnabled(bool bEnable);
	void StartWave(int WaveIndex);
	void StopAllSpawns();

	void OnEnemySpawned();

private:
	UPROPERTY()
	TArray<AEnemySpawner*> SpawnerList;

	int CurrentWave = 0;

	int AliveEnemyCount = 0;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

};
