// Fill out your copyright notice in the Description page of Project Settings.


#include "Datas/BarBaseActor.h"

// Sets default values
ABarBaseActor::ABarBaseActor()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void ABarBaseActor::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ABarBaseActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

