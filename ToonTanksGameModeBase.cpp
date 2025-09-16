// Fill out your copyright notice in the Description page of Project Settings.


#include "ToonTanksGameModeBase.h"
#include "Kismet/GameplayStatics.h"
#include "Tank.h"
#include "Tower.h"
#include "ToonTanksPlayerController.h"
#include "GameFramework/PlayerController.h"
#include "SpawnManager.h"
#include "BasePawn.h"
#include "GameFramework/Pawn.h"


void AToonTanksGameModeBase::ActorDied(AActor* DeadActor)
{
	ABasePawn* DeadPawn = Cast<ABasePawn>(DeadActor);
	if (!DeadPawn)
	{
		UE_LOG(LogTemp, Warning, TEXT("InValid Actor Dead!!?"))
		return;
	}

	if (APlayerController* PC = Cast<APlayerController>(DeadPawn->GetController()))
	{
		HandlePlayerDeath();
	}
	else
	{
		HandleNPCDeath(DeadPawn);
	}
}

void AToonTanksGameModeBase::HandlePlayerDeath()
{
	PlayerTank->HandlePlayerDestruction();
	if (ToonTanksPlayerController)
	{
		ToonTanksPlayerController->SetPlayerEnabledState(false);
	}
	GameOver(false);
}

void AToonTanksGameModeBase::HandleNPCDeath(ABasePawn* DestroyedPawn)
{
	DestroyedPawn->HandleDestruction();
	//WARNING::DestroyedPawn has been set to null
	--TargetTowers;

	if (TargetTowers > 0) return;

	if (remainWave == 0)
	{
		GameOver(true);
	}
	else
	{
		remainWave--;
		StartNextWave();
	}
}

void AToonTanksGameModeBase::StartNextWave()
{
	if (SpawnManager)
	{
		SpawnManager->StartWave(++currentWave);
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("Caution! missing SpawnManager"));
	}
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


	
	//TODO:More Wave!!

}

void AToonTanksGameModeBase::StartWave()
{
	SpawnManager = GetWorld()->SpawnActor<ASpawnManager>(SpawnManagerClass, FVector::ZeroVector, FRotator::ZeroRotator);
	if (SpawnManager)
	{
		SpawnManager->OnWaveStart.AddDynamic(this, &AToonTanksGameModeBase::HandleWaveStart);
		SpawnManager->StartWave(currentWave);

	}
}


void AToonTanksGameModeBase::HandleGameStart()
{
	TargetTowers = GetTargetTowerCount();
	PlayerTank = Cast<ATank>(UGameplayStatics::GetPlayerPawn(this, 0));
	ToonTanksPlayerController = Cast<AToonTanksPlayerController>(UGameplayStatics::GetPlayerController(this, 0));


	StartGame();

	if (ToonTanksPlayerController)
	{
		ToonTanksPlayerController->SetPlayerEnabledState(false);

		FTimerHandle StartDelayTimerHandle;
		FTimerDelegate StartDelayDelegate;

		StartDelayDelegate.BindLambda([this]()
			{
				if (ToonTanksPlayerController)
				{
					ToonTanksPlayerController->SetPlayerEnabledState(true);
				}
				StartWave();
			});

		GetWorldTimerManager().SetTimer(
			StartDelayTimerHandle,
			StartDelayDelegate,
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
