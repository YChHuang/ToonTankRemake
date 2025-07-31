// Fill out your copyright notice in the Description page of Project Settings.


#include "FireProjectTile.h"

// Sets default values
AFireProjectTile::AFireProjectTile()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	ProjectTileMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("ProjectTileMesh"));
	RootComponent = ProjectTileMesh;


}

// Called when the game starts or when spawned
void AFireProjectTile::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AFireProjectTile::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

