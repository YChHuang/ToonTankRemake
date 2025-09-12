// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "SpawnManager.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnWaveStart, int32, WaveIndex);


UCLASS()
class TOONTANKS_API ASpawnManager : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ASpawnManager();
	void RegisterSpawner(class AEnemySpawner* Spawner);

	void StartWave(int WaveIndex);


	UPROPERTY(BlueprintAssignable)
	FOnWaveStart OnWaveStart;

	void handleWave();

	void OnEnemySpawned();

private:
	UPROPERTY()
	TArray<AEnemySpawner*> SpawnerList;

	UFUNCTION()
	void handleEnemySpawn(class ABasePawn* SpawnedEnemy);
	FTimerHandle WaveTimerHandle;

	int32 currentWave = 0;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;


public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

};
