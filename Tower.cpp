// Fill out your copyright notice in the Description page of Project Settings.


#include "Tower.h"
#include "Tank.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/Actor.h"


void ATower::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);


	if (InFireRange())
	{
		RotateTurret(Tank->GetActorLocation());
	}
}

void ATower::HandleDestruciton()
{
	Super::HandleDestruction();
	Destroy();
}

void ATower::BeginPlay()
{
	Super::BeginPlay();

	Tank = Cast<ATank>(UGameplayStatics::GetPlayerPawn(this, 0));

	GetWorldTimerManager().SetTimer(FireRateTimerHandle, this, &ATower::CheckFireCondition, FireRate, true);
}

bool ATower::InFireRange()
{
	if (Tank)
	{
		float Distance = FVector::Dist(GetActorLocation(), Tank->GetActorLocation());
		// Check to see if the tank is in range
		if (Distance < FireRange)
		{
			// if in rnage, rotate turret toward tank
			return true;
		}

	}
	return false;
}

void ATower::CheckFireCondition()
{
	if (!Tank) 
	{
		return;
	}
	if (InFireRange() && Tank->bAlive) 
	{
		Fire();
	}
}
