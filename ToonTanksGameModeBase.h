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

	void addTower();

	void TowerHasSpawned();

	void RegisterSpawner(AEnemySpawner* Spawner);

	TArray<AEnemySpawner*> SpawnerList;


protected:

	virtual void BeginPlay() override;


	UFUNCTION(BlueprintImplementableEvent)
	void StartGame();

	UFUNCTION(BlueprintImplementableEvent)
	void GameOver(bool bWonGame);

	
	
private:
	class ASpawnManager* Manager;
	class ATank* Tank;
	class AToonTanksPlayerController* ToonTanksPlayerController;

	float StartDelay = 3.f;

	void HandleGameStart();

	int32 TargetTowers = 0;
	int32 MaxSpawnCount = 0;
	
	int GetTargetTowerCount();

	
	

};
