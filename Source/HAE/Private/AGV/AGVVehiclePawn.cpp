// Fill out your copyright notice in the Description page of Project Settings.

#include "AGV/AGVVehiclePawn.h"
#include "AGV/AGVLog.h"
#include "AGV/AGVDataContainer.h"
#include "Components/StaticMeshComponent.h"
#include "ChaosWheeledVehicleMovementComponent.h"
#include "ChaosVehicles/Public/ChaosVehicleWheel.h"
#include "ChaosVehicleMovementComponent.h"
#include "EnhancedInput/Public/EnhancedInputComponent.h"
#include "EnhancedInput/Public/EnhancedInputSubsystems.h"


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

	// Enhanced Input Subsystem ��������
	APlayerController* PlayerController = Cast<APlayerController>(GetController());
	if (PlayerController)
	{
		UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer());
		if (Subsystem && PlayerInputMappingContext)
		{
			// Input Mapping Context �߰�
			Subsystem->AddMappingContext(PlayerInputMappingContext, 0);
		}
	}
}

// �� �޽� ���Ͽ� ����
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

// ��ǲ
void AAGVVehiclePawn::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(PlayerInputComponent);
	if (EnhancedInputComponent)
	{
		// Throttle �Լ� ���ε�
		EnhancedInputComponent->BindAction(ThrottleAction, ETriggerEvent::Triggered, this, &AAGVVehiclePawn::HandleThrottleInput);
		EnhancedInputComponent->BindAction(ThrottleAction, ETriggerEvent::Completed, this, &AAGVVehiclePawn::HandleThrottleRelease);

		// Steering �Լ� ���ε�
		EnhancedInputComponent->BindAction(SteeringAction, ETriggerEvent::Triggered, this, &AAGVVehiclePawn::HandleSteeringInput);
		EnhancedInputComponent->BindAction(SteeringAction, ETriggerEvent::Completed, this, &AAGVVehiclePawn::HandleSteeringRelease);

		// �극��ũ �Լ� ���ε�
		EnhancedInputComponent->BindAction(BrakeAction, ETriggerEvent::Triggered, this, &AAGVVehiclePawn::HandleBrakeInput);
		EnhancedInputComponent->BindAction(BrakeAction, ETriggerEvent::Completed, this, &AAGVVehiclePawn::HandleBrakeRelease);
	}
}

// ���� �Լ�
void AAGVVehiclePawn::HandleThrottleInput(const FInputActionValue& Value)
{
	float ThrottleValue = Value.Get<float>();

	if (GetVehicleMovementComponent())
	{
		if (ThrottleValue >= 0)
		{
			GetVehicleMovementComponent()->SetTargetGear(1, true);
			GetVehicleMovementComponent()->SetThrottleInput(ThrottleValue);
		}
		else
		{
			GetVehicleMovementComponent()->SetTargetGear(-1, true);
			GetVehicleMovementComponent()->SetThrottleInput(FMath::Abs(ThrottleValue));
		}
	}
}

// ����Ʋ ����
void AAGVVehiclePawn::HandleThrottleRelease(const FInputActionValue& Value)
{
	if (GetVehicleMovementComponent())
	{
		// �Է� ���� �� ����Ʋ ����
		GetVehicleMovementComponent()->SetThrottleInput(0.0f); 
	}
}

// �ڵ� ����
void AAGVVehiclePawn::HandleSteeringInput(const FInputActionValue& Value)
{
	float SteeringValue = Value.Get<float>();
	if (GetVehicleMovementComponent())
	{
		GetVehicleMovementComponent()->SetSteeringInput(SteeringValue);
	}
}

// �ڵ� ���� ����
void AAGVVehiclePawn::HandleSteeringRelease(const FInputActionValue& Value)
{
	if (GetVehicleMovementComponent())
	{
		GetVehicleMovementComponent()->SetSteeringInput(0.0f); // �Է� ���� �� ���� ����
	}
}

// �극��ũ �۵�
void AAGVVehiclePawn::HandleBrakeInput(const FInputActionValue& Value)
{
	float BrakeValue = Value.Get<float>();
	if(GetVehicleMovementComponent())
	{
		GetVehicleMovementComponent()->SetBrakeInput(BrakeValue);
	}
}

// �극��ũ ����
void AAGVVehiclePawn::HandleBrakeRelease(const FInputActionValue& Value)
{
	if (GetVehicleMovementComponent())
	{
		GetVehicleMovementComponent()->SetBrakeInput(0);
	}
}

void AAGVVehiclePawn::SetAGVData(UAGVDataContainer* NewDataContainer)
{
	AGVDataContainer = NewDataContainer;
}