// Fill out your copyright notice in the Description page of Project Settings.


#include "Healthcomponent.h"
#include "Kismet/GameplayStatics.h"
#include "ToonTanksGameModeBase.h"

// Sets default values for this component's properties
UHealthcomponent::UHealthcomponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}


// Called when the game starts
void UHealthcomponent::BeginPlay()
{
	Super::BeginPlay();

	Health = MaxHealth;

	GetOwner()->OnTakeAnyDamage.AddDynamic(this, &UHealthcomponent::Damagetaken);
	
	ToonTanksGameModeBase = Cast<AToonTanksGameModeBase>(UGameplayStatics::GetGameMode(this));
}




// Called every frame
void UHealthcomponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}

void UHealthcomponent::Damagetaken(AActor* DamagedActor, float Damage, const UDamageType* DamageType, AController* Instigator, AActor* DamageCauser)
{
	if (Damage <= 0.f) return;

	Health -= Damage;
	if (Health <= 0.f && ToonTanksGameModeBase)
	{
		ToonTanksGameModeBase->ActorDied(DamagedActor);
	}
}