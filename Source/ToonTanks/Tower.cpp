// Fill out your copyright notice in the Description page of Project Settings.


#include "Tower.h"
#include "Tank.h"
//#include "Kismet/GameplayStatics.h"
#include "GameFramework/Actor.h"
#include "AIController.h"


void ATower::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void ATower::HandleDestruction()
{
	Super::HandleDestruction();
}

void ATower::BeginPlay()
{
	Super::BeginPlay();


	if (AutoControllerClass)
	{

		//UE_LOG(LogTemp, Display, TEXT("Get controller"));
		AAIController* AIController = GetWorld()->SpawnActor<AAIController>(AutoControllerClass);
		if (AIController)
		{
			AIController->Possess(this);
		}

	}
}
