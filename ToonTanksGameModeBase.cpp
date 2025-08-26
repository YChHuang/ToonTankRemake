// Fill out your copyright notice in the Description page of Project Settings.


#include "ToonTanksGameModeBase.h"
#include "Kismet/GameplayStatics.h"
#include "Tank.h"
#include "Tower.h"
#include "ToonTanksPlayerController.h"
#include "EnemySpawner.h"
#include "SpawnManager.h"

void AToonTanksGameModeBase::ActorDied(AActor* DeadActor)
{
	if (DeadActor == Tank)
	{
		Tank->HandleDestruction();
		if (ToonTanksPlayerController)
		{
			ToonTanksPlayerController->SetPlayerEnabledState(false);
		}
		GameOver(false);
	}
	else if (ATower* DestroyedTower = Cast<ATower>(DeadActor))
	{
		DestroyedTower->HandleDestruciton();
		--TargetTowers;
		//Won condition : towers that is already in world is zero and will not spawn any tower
		if (TargetTowers == 0)
		{
			GameOver(true);
		}
	}
}

void AToonTanksGameModeBase::addTower()
{
	TargetTowers++;//Adding Tower count
}

//void AToonTanksGameModeBase::RegisterSpawner(AEnemySpawner* Spawner)
//{
//	//get the spawner
//	SpawnerList.Add(Spawner);
//}
//
//void AToonTanksGameModeBase::TowerHasSpawned()
//{
//	MaxSpawnCount--;
//}

void AToonTanksGameModeBase::BeginPlay()
{
	Super::BeginPlay();

	HandleGameStart();

	Manager = Cast<ASpawnManager>(
		UGameplayStatics::GetActorOfClass(GetWorld(), ASpawnManager::StaticClass()));

	if (Manager)
	{
		UE_LOG(LogTemp, Warning, TEXT("Got manager"))
		Manager->StartWave(1); // ŠJ‘æˆê”g
	}

}


void AToonTanksGameModeBase::OnWaveEnd()
{
	if (Manager)
	{
		Manager->StopAllSpawns();
	}
}


void AToonTanksGameModeBase::HandleGameStart()
{
	TargetTowers = GetTargetTowerCount();
	Tank = Cast<ATank>(UGameplayStatics::GetPlayerPawn(this, 0));
	ToonTanksPlayerController = Cast<AToonTanksPlayerController>(UGameplayStatics::GetPlayerController(this, 0));

	StartGame();

	if (ToonTanksPlayerController)
	{
		ToonTanksPlayerController->SetPlayerEnabledState(false);

		FTimerHandle PlayerEnableTimerHandle;
		FTimerDelegate PlayerEnableTimerDelegate = FTimerDelegate::CreateUObject(
			ToonTanksPlayerController, 
			&AToonTanksPlayerController::SetPlayerEnabledState, 
			true
		);
		GetWorldTimerManager().SetTimer(
			PlayerEnableTimerHandle,
			PlayerEnableTimerDelegate,
			StartDelay,
			false
		);
	}
}

int AToonTanksGameModeBase::GetTargetTowerCount()
{
	TArray<AActor*> Towers;
	UGameplayStatics::GetAllActorsOfClass(this, ATower::StaticClass(), Towers);

	return Towers.Num();
}
