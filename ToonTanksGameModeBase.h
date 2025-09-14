// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "EnemySpawner.h"
#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "ToonTanksGameModeBase.generated.h"


/**
 * 
 */
UCLASS()
class TOONTANKS_API AToonTanksGameModeBase : public AGameModeBase
{
	GENERATED_BODY()

public:

	void ActorDied(AActor* DeadActor);

	void addTower(int amount);


	void TowerHasSpawned();

	void RegisterSpawner(AEnemySpawner* Spawner);

	TArray<AEnemySpawner*> SpawnerList;

	UFUNCTION()
	void HandleWaveStart(int32 WaveEnemyRemain);


protected:

	virtual void BeginPlay() override;


	UFUNCTION(BlueprintImplementableEvent)
	void StartGame();

	UFUNCTION(BlueprintImplementableEvent)
	void GameOver(bool bWonGame);

	UPROPERTY(EditDefaultsOnly, Category = "Spawn")
	TSubclassOf<class ASpawnManager> SpawnManagerClass;
	
private:
	class ASpawnManager* Manager;
	class ATank* Tank;
	class AToonTanksPlayerController* ToonTanksPlayerController;

	class ASpawnManager* SpawnManager;

	float StartDelay = 3.f;

	void HandleGameStart();

	int32 TargetTowers = 0;
	int32 MaxSpawnCount = 0;
	
	int GetTargetTowerCount();

	//TODO deletit
	//int RemainingSpawns = 0;
	

};
