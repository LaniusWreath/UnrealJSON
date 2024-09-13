// Fill out your copyright notice in the Description page of Project Settings.


#include "Actors/GetHttpActor.h"

#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"

// Sets default values
AGetHttpActor::AGetHttpActor()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	OverlapComponent = CreateDefaultSubobject<UBoxComponent>("Overlap");
	SetRootComponent(OverlapComponent);

	ResponseWidget = CreateDefaultSubobject<UWidgetComponent>("Response Widget");
	ResponseWidget->SetupAttachment(OverlapComponent);

	Http = &FHttpModule::Get();
}

// Called when the game starts or when spawned
void AGetHttpActor::BeginPlay()
{
	Super::BeginPlay();

	OverlapComponent->OnComponentBeginOverlap.AddDynamic(this, &ThisClass::OnBeginOverlap);
}

// Called every frame
void AGetHttpActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (ResponseWidget)
	{
		APlayerCameraManager* CameraManager = UGameplayStatics::GetPlayerCameraManager(GetWorld(), 0);
		if (CameraManager)
		{
			FRotator LookAtRotation = UKismetMathLibrary::FindLookAtRotation(CameraManager->GetCameraLocation(), ResponseWidget->GetComponentLocation());
			LookAtRotation.Pitch = 0.f;
			LookAtRotation.Roll = 0.f;
			LookAtRotation.Yaw -= 180.f;
			ResponseWidget->SetWorldRotation(LookAtRotation);
		}
	}
}

void AGetHttpActor::OnBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, 
	int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
}

