// Fill out your copyright notice in the Description page of Project Settings.


#include "ToonTanksPlayerController.h"
#include "GameFramework/Pawn.h"
#include "GameFramework/PlayerController.h"


void AToonTanksPlayerController::SetPlayerEnabledState(bool bPlayerEnabled)
{
	if (bPlayerEnabled)
	{
		GetPawn()->EnableInput(this);
	}
	else
	{
		GetPawn()->DisableInput(this);
	}
	//bShowMouseCursor = bPlayerEnabled;
}
void AToonTanksPlayerController::BeginPlay()
{
	Super::BeginPlay();
	bShowMouseCursor = false;

	FInputModeGameOnly InputMode;
	SetInputMode(InputMode);

	// İ’è—A“ü–Í®ˆ×—VE–Í®



}
	