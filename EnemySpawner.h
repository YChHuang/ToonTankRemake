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

	class ABasePawn* SpawnEnemy();

	


protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UPROPERTY(EditAnywhere)
	TSubclassOf<ABasePawn> EnemyClass;



	UPROPERTY(EditAnywhere)
	int32 MaxEnemies = 10;


	

private:
	class AToonTanksGameModeBase* GameModeBase;

	int Towers = 0;

	bool SpawnEnable = false;

	class ASpawnManager* Manager;

	int spawnCount = 5;

	FVector GetSpawnLocation();

	float GetCapsuleHeightOffset(TSubclassOf<ABasePawn> Class);

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

};
