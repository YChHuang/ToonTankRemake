// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "EnemySpawner.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnEnemySpawned, ABasePawn*, SpawnedEnemy);

/*EnemySpawner-敵人生成器
* 職責：
*	向下掃描地板取得生成座標
*	可指定不同敵人子類別
*	使用硬編碼維護各Enemy的高度偏差(HACK)
*	SpawnManger有訂閱OnEnemySpawned
* 使用方法：
*	1.在Editor放入該BP派生類
*	2.GameMode會工廠生成SpawnManager，之後manager會掃描所有的spawner並存在容器
*	3.在editor中透過BP指派EnemyClass
*/

UCLASS()
class TOONTANKS_API AEnemySpawner : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AEnemySpawner();

	//Delegate的事件
	UPROPERTY(BlueprintAssignable, Category = "Events")
	FOnEnemySpawned OnEnemySpawned; // 事件對外公開


	void SpawnEnemy();

	


protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UPROPERTY(EditAnywhere)
	TSubclassOf<ABasePawn> EnemyClass;

	// SpawnManager.h
	UPROPERTY(EditDefaultsOnly, Category = "Spawn")
	TMap<TSubclassOf<AActor>, float> EnemyHeightOffsets;

private:

	bool SpawnEnable = false;


	int spawnCount = 5;

	FVector GetSpawnLocation();


	ABasePawn* SpawnedEnemy;


public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

};
