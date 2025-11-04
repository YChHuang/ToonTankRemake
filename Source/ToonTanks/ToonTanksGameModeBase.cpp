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
		UE_LOG(LogTemp, Warning, TEXT("destroyed enemy"))
		HandleNPCDeath(DeadPawn);
	}
}

//Šß‰ÆŽ€–Sç´S
void AToonTanksGameModeBase::HandlePlayerDeath()
{
	//ŒÄ‹©Pawn“IŽ©šÊ
	PlayerTank->HandlePlayerDestruction();
	//Ÿ“¾T§žÜ
	if (ToonTanksPlayerController)
	{
		ToonTanksPlayerController->SetPlayerEnabledState(false);
	}
	//GameOver:: false == lose, true == win
	GameOver(false);
}

void AToonTanksGameModeBase::HandleNPCDeath(ABasePawn* DestroyedPawn)
{
	if (!DestroyedPawn) return;
	DestroyedPawn->HandleDestruction();
	//WARNING::DestroyedPawn has been set to null
	--EnemyRemainCount;

	if (EnemyRemainCount > 0) return;

	if (RemainWave == 0)
	{
		GameOver(true);
	}
	else
	{
		RemainWave--;
		StartNextWave();
	}
}

void AToonTanksGameModeBase::StartNextWave()
{
	if (SpawnManager)
	{
		SpawnManager->StartWave(++CurrentWave);
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("Caution! missing SpawnManager"));
	}
}



void AToonTanksGameModeBase::HandleWaveStart(int32 WaveEnemyRemain)
{
	EnemyRemainCount += WaveEnemyRemain;
	UE_LOG(LogTemp, Warning, TEXT("This wave will gonna spawn %d s Enemy"), EnemyRemainCount)
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
		SpawnManager->StartWave(CurrentWave);

	}
}


void AToonTanksGameModeBase::HandleGameStart()
{
	EnemyRemainCount = GetEnemyCount();
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

int AToonTanksGameModeBase::GetEnemyCount()
{
	TArray<AActor*> Enemies;
	UGameplayStatics::GetAllActorsOfClass(this, ABasePawn::StaticClass(), Enemies);
	int count = 0;
	for (AActor* Enemy : Enemies) {
		if (Enemy && Enemy->ActorHasTag("Enemy")) count++;
	}

	return count;
}
