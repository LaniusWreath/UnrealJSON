// Fill out your copyright notice in the Description page of Project Settings.

#include "AGV/AGVVehiclePawn.h"
#include "AGV/AGVLog.h"
#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"
#include "Components/StaticMeshComponent.h"
#include "ChaosWheeledVehicleMovementComponent.h"
#include "ChaosVehicles/Public/ChaosVehicleWheel.h"
#include "ChaosVehicleMovementComponent.h"
#include "AGV/AGVDataContainer.h"

AAGVVehiclePawn::AAGVVehiclePawn()
{
	WheelFL = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Wheel FL"));
	WheelFL->SetupAttachment(RootComponent);

	WheelFR = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Wheel FR"));
	WheelFR->SetupAttachment(RootComponent);

	WheelRL =CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Wheel RL"));
	WheelRL->SetupAttachment(RootComponent);

	WheelRR =CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Wheel RR"));
	WheelRR->SetupAttachment(RootComponent);

}

void AAGVVehiclePawn::BeginPlay()
{
	Super::BeginPlay();
	// ���� ���̷�Ż �޽� ���Ͽ� Ÿ�̾� ����ƽ �޽� ����
	AttachWheelMeshToSocket();
}

void AAGVVehiclePawn::UpdateVehicleProperties(UAGVDataContainer* InAGVDataContainer)
{
	AGVDataContainer = InAGVDataContainer;
}

void AAGVVehiclePawn::AttachWheelMeshToSocket()
{
	if (const USkeletalMeshSocket* Socket = GetMesh()->GetSocketByName(FName("Socket_RR")))
	{
		WheelRR->AttachToComponent(GetMesh(), FAttachmentTransformRules::SnapToTargetNotIncludingScale, FName("VisWheel_BR"));
	}

	if (const USkeletalMeshSocket* Socket = GetMesh()->GetSocketByName(FName("Socket_FR")))
	{
		WheelFR->AttachToComponent(GetMesh(), FAttachmentTransformRules::SnapToTargetNotIncludingScale, FName("VisWheel_FR"));
	}

	if (const USkeletalMeshSocket* Socket = GetMesh()->GetSocketByName(FName("Socket_RL")))
	{
		WheelRL->AttachToComponent(GetMesh(), FAttachmentTransformRules::SnapToTargetNotIncludingScale, FName("VisWheel_BL"));
	}

	if (const USkeletalMeshSocket* Socket = GetMesh()->GetSocketByName(FName("Socket_FL")))
	{
		WheelFL->AttachToComponent(GetMesh(), FAttachmentTransformRules::SnapToTargetNotIncludingScale, FName("VisWheel_FL"));
	}
}

void AAGVVehiclePawn::UpdateVehiclePosition(const FVector& TargetLocation, float TargetYaw)
{
	FVector CurrentLocation = GetActorLocation();
	FVector Direction = (TargetLocation - CurrentLocation).GetSafeNormal();
	
	// ���� ������ ��ũ �� ��������
	float CurrentEngineTorque = GetEngineTorque();

}

float AAGVVehiclePawn::GetEngineTorque()
{
	UChaosVehicleMovementComponent* MovementComponent = GetVehicleMovementComponent();
	// Chaos Vehicle�� ���� ������Ʈ���� ���� ��ũ �� ��������
	if (MovementComponent && MovementComponent->)
	{
		return ChaosVehicle->VehicleComponent->GetEngineTorque();
	}

	// �⺻�� ��ȯ (���� ��ũ�� ���� ���)
	return 0.0f;
}

void AAGVVehiclePawn::SetAGVData(UAGVDataContainer* NewDataContainer)
{
	AGVDataContainer = NewDataContainer;
}


void AAGVVehiclePawn::MoveForward(float Value)
{
}

void AAGVVehiclePawn::MoveRight(float Value)
{
}

void AAGVVehiclePawn::ApplyHandbrake(bool bPressed)
{
}
