// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "EnemySpawner.h"
#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "ToonTanksGameModeBase.generated.h"

/*ToonTanksGameModeBase - GameMode
* 職責：
*	處理玩家死亡
*	處理NPC死亡
*	控制勝利條件
*/


//
//  ToonTanksGameModeBase - GameMode入口
// 
//  職責:
//  - 處理玩家死亡
//  - 處理勝利條件
//  - 波次入口
// 
//  使用方式:
//  1. 在Editor套用此方法
// 
//  玩家死亡邏輯：
//	 -在ATank呼叫destroy
//	  -沒收控制器控制權
//		-玩家輸掉遊戲
// 
//	勝利條件：
//	 -玩家存活且場上沒敵人且波次已完成
//	  -
UCLASS()
class TOONTANKS_API AToonTanksGameModeBase : public AGameModeBase
{
	GENERATED_BODY()

public:

	void ActorDied(AActor* DeadActor);


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
	class ATank* PlayerTank;
	class AToonTanksPlayerController* ToonTanksPlayerController;

	class ASpawnManager* SpawnManager;

	float StartDelay = 3.f;

	void HandleGameStart();

	void StartWave();

	int32 EnemyRemainCount = 0;
	int32 MaxSpawnCount = 0;
	UPROPERTY(EditAnywhere, Category = "Spawn")
	int32 RemainWave = 2;//Using this to decide how many wave will be start, but is not a good way.
	int32 CurrentWave = 0;
	
	int GetEnemyCount();
	
	void HandlePlayerDeath();

	void HandleNPCDeath(class ABasePawn* DestroyedPawn);

	void StartNextWave();

};
