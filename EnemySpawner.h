// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "EnemySpawner.generated.h"

UCLASS()
class TOONTANKS_API AEnemySpawner : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AEnemySpawner();

	void DisableSpawn();
	void SetSpawnEnable(bool bEnable);

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UPROPERTY(EditAnywhere)
	TSubclassOf<class ATower> EnemyClass;

	UPROPERTY(EditAnywhere)
	float SpawnInterval = 5.0f;

	UPROPERTY(EditAnywhere)
	int32 MaxEnemies = 10;

	FTimerHandle SpawnTimer;

	void SpawnEnemy();

private:
	class AToonTanksGameModeBase* GameModeBase;

	int Towers = 0;

	bool SpawnEnable = false;

	class ASpawnManager* Manager;

	int spawnCount = 5;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

};
