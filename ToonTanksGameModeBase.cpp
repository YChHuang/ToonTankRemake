// Fill out your copyright notice in the Description page of Project Settings.


#include "ToonTanksGameModeBase.h"
#include "Kismet/GameplayStatics.h"
#include "Tank.h"
#include "Tower.h"
#include "ToonTanksPlayerController.h"
#include "EnemySpawner.h"
#include "SpawnManager.h"
#include "GameFramework/PlayerController.h"

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
		
		//Won condition : No Towers remain and no more towers will spawn
		if (TargetTowers == 0 && RemainingSpawns == 0)
		{
			GameOver(true);
		}
	}
}

void AToonTanksGameModeBase::addTower(int amount)
{
	TargetTowers += amount;//Adding Tower spawn count
	
	//UE_LOG(LogTemp, Warning, TEXT("Still remain %d Towers!!"), TargetTowers)
}



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
