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

	UFUNCTION()
	void handleEnemySpawn(class ABasePawn* SpawnedEnemy);


protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;


public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

};
