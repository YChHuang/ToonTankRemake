// Fill out your copyright notice in the Description page of Project Settings.


#include "ToonTanksGameModeBase.h"
#include "Kismet/GameplayStatics.h"
#include "Tank.h"
#include "Tower.h"
#include "ToonTanksPlayerController.h"
#include "GameFramework/PlayerController.h"
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
		
		//Won condition : No Towers remain and no more towers will spawn
		if (TargetTowers == 0)
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



void AToonTanksGameModeBase::HandleWaveStart(int32 WaveEnemyRemain)
{
	TargetTowers += WaveEnemyRemain;
	UE_LOG(LogTemp, Warning, TEXT("This wave will gonna spawn %d s Enemy"), TargetTowers)
}

void AToonTanksGameModeBase::BeginPlay()
{
	Super::BeginPlay();

	HandleGameStart();

	SpawnManager = GetWorld()->SpawnActor<ASpawnManager>(SpawnManagerClass, FVector::ZeroVector, FRotator::ZeroRotator);
	if (SpawnManager)
	{
		SpawnManager->OnWaveStart.AddDynamic(this, &AToonTanksGameModeBase::HandleWaveStart);
		SpawnManager->StartWave(0);
		
	}
	
	//TODO:More Wave!!

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
