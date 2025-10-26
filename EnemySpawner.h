// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "EnemySpawner.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnEnemySpawned, ABasePawn*, SpawnedEnemy);



UCLASS()
class TOONTANKS_API AEnemySpawner : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AEnemySpawner();

	//DelegateìIéñåè
	UPROPERTY(BlueprintAssignable, Category = "Events")
	FOnEnemySpawned OnEnemySpawned; // éñåèõîäOåˆäJ


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
