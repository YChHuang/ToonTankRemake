// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "SpawnManager.generated.h"

//
// SpawnManager - 波次生成管理器
// 
//  職責:
//  - 管理多個 EnemySpawner
//  - 根據波次配置定時生成敵人
//  - 追蹤生成進度並通知 GameMode
// 
//  使用方式:
//  1. 在關卡中放置多個 EnemySpawner
//  2. GameMode 呼叫 StartWave(WaveIndex)
//  3. 透過 OnWaveStart/OnEnemySpawned 接收事件
// 
//  波次流程:
//  GameMode::StartWave()
//    → SpawnManager::StartWave()
//      → 啟動 Timer (每 SpawnInterval 秒)
//        → SpawnEnemyTick()
//          → 隨機選 Spawner
//          → Spawner::SpawnEnemy()
//            → 觸發 OnEnemySpawned
 

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnWaveStart, int32, WaveEnemies);


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

	//Container which stores spawners. It will add spawner while start game
	UPROPERTY()
	TArray<AEnemySpawner*> SpawnerList;

	UFUNCTION()
	void handleEnemySpawn(class ABasePawn* SpawnedEnemy);
	FTimerHandle WaveTimerHandle;

	int32 CurrentWave = 0;

	//HACK::Hard core
	TArray<int32> WaveConfigs = {5, 5, 8, 8};

	int CurrentEnemyCount = 0;

	UPROPERTY(EditAnyWhere, Category = "Spawn")
	float SpawnInterval = 2.f;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;


public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

};
