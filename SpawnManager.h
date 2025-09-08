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
	void StartWave(int WaveIndex);

	

	void OnEnemySpawned();

private:
	UPROPERTY()
	TArray<AEnemySpawner*> SpawnerList;

	int CurrentWave = 0;

	int AliveEnemyCount = 0;

	FTimerHandle SpawnTimerHandle;

	void SpawnTick();

	int RemainingSpawns = 0;

	class AToonTanksGameModeBase* GameMode;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UPROPERTY(EditAnywhere, Category = "Spawn")
	float SpawnInterval = 5.0f;

	UPROPERTY(EditAnywhere, Category = "Spawn")
	int32 SpawnTypeIndex = 0;


public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

};
