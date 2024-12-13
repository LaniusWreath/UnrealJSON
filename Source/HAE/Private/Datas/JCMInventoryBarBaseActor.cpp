// Fill out your copyright notice in the Description page of Project Settings.


#include "Datas/JCMInventoryBarBaseActor.h"
#include "Datas/JCMLog.h"
#include "Components/TextRenderComponent.h"
#include "Datas/StaticMeshInventory.h"


AJCMInventoryBarBaseActor::AJCMInventoryBarBaseActor()
{
	// ������ �޽� ������Ʈ �ʱ�ȭ
	TemplateItemMeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("InventoryItemStaticMeshComponent"));
	TemplateItemMeshComponent->SetupAttachment(CustomStaticMeshTemplateComponent);

	// �ؽ�Ʈ ������ - ��
	TextRenderComponentSafeValue = CreateDefaultSubobject<UTextRenderComponent>(TEXT("TextRenderComponentMaxValue"));
	TextRenderComponentSafeValue->SetupAttachment(RootComponent);

	// �ؽ�Ʈ �ǹ��� ����� ����
	TextRenderComponentSafeValue->SetHorizontalAlignment(EHorizTextAligment::EHTA_Center);
	TextRenderComponentSafeValue->SetVerticalAlignment(EVerticalTextAligment::EVRTA_TextCenter);

}

// �Է¹��� ����ƽ �޽� ������Ʈ�� �κ��丮 �޽� ���� : �Ⱦ�.
void AJCMInventoryBarBaseActor::InitializeStaticMeshPropertyFromInventory(UStaticMeshComponent* TargetStaticMeshComponent, const int32 InInventoryIndex)
{
	UStaticMesh* TemplateStaticMesh = GetStaticMeshFromInventory(InInventoryIndex);

	if (!TemplateStaticMesh)
	{
		UE_LOG(JCMlog, Error, TEXT("%s : Invalid StaticMesh"), *this->GetName());
		return;
	}

	TargetStaticMeshComponent->SetStaticMesh(TemplateStaticMesh);
}

// �Է¹��� �ؽ�Ʈ ���� ������Ʈ�� �κ��丮 MaxValue ����
void AJCMInventoryBarBaseActor::InitializeTextRenderComponentMaxValue(UTextRenderComponent* TargetTextRenderComponent,
	const int32 InInventoryIndex)
{
	if (!InventoryDataAsset)
	{
		UE_LOG(JCMlog, Error, TEXT("%s : Inventory Data Asset is invalid"), *this->GetName());
		return;
	}

	int32 Amount = InventoryDataAsset->GetAmount(InInventoryIndex);
	TargetTextRenderComponent->SetText(FText::AsNumber(Amount));

	// ��������Ʈ ���ε�

}


// ��������Ʈ�� ���ε� �� �Լ� ����
void AJCMInventoryBarBaseActor::OnChartGeneratingDoneBindingRoutine()
{
	SafeAmountAlarmRoutine();
}

// �˶� ��� �Լ� ����
void AJCMInventoryBarBaseActor::SafeAmountAlarmRoutine()
{

	int32 SafeAmount = GetValueFromTextRenderComponent(TextRenderComponentSafeValue);
	int32 CurrentAmount = GetValueFromTextRenderComponent(TextRenderComponentValue);

	// ù ��° ������ ����ƽ �޽� ������Ʈ ����
	UStaticMeshComponent* TargetStaticMeshComponent = GetCustomStaticMeshComponent(ParentSplineIndex);
	if (!TargetStaticMeshComponent)
	{
		UE_LOG(JCMlog, Error, TEXT("SafeAmountAlarmRoutine : couldn't find GeneratedStaticMeshComponent"));
		return;
	}

	// ���� �������� ������ ���� ������ ����
	if (CurrentAmount > SafeAmount)
	{
		// ��Ƽ���� �⺻���� ����
		if (!DefaultMaterial)
		{
			UE_LOG(JCMlog, Error, TEXT("SafeAmountAlarmRoutine : couldn't find DefaultMaterial"));
			return;
		}
		ChangeStaticMeshComponentMaterial(TargetStaticMeshComponent, DefaultMaterial);
		return;
	}
	else
	{
		if (!AlarmMaterial)
		{
			UE_LOG(JCMlog, Error, TEXT("SafeAmountAlarmRoutine : couldn't find AlarmMaterial"));
			return;
		}

		// ��Ƽ���� �˶������� ����
		ChangeStaticMeshComponentMaterial(TargetStaticMeshComponent, AlarmMaterial);
	}
}

UStaticMeshComponent* AJCMInventoryBarBaseActor::ChangeStaticMeshComponentMaterial(UStaticMeshComponent* TargetStaticMeshComponent,
	UMaterialInterface* InMaterial)
{
	if (!TargetStaticMeshComponent)
	{
		UE_LOG(JCMlog, Error, TEXT("ChangeStaticMeshComponentMaterial : TargetStaticMeshComponent is invalid"));
		return nullptr;
	}
	if (!InMaterial)
	{
		UE_LOG(JCMlog, Error, TEXT("ChangeStaticMeshComponentMaterial : InMaterial is invalid"));
		return nullptr;
	}
	
	// �̺κ� ��Ƽ���� 0�� ���Ը� �ٲٵ��� �ϵ��ڵ� �ߴµ�, ���� ���� �� ����.
	TargetStaticMeshComponent->SetMaterial(0, InMaterial);
	return nullptr;
}

// ���� �˻�
void AJCMInventoryBarBaseActor::CheckSafeAmount(const int32 InSafeAmount, const int32 InCurrentAmount)
{
	/*if (InSafeAmount > InCurrentAmount)
	{
		CustomActorSceneComponent->GetChildrenComponents();
	}*/
}


// �κ��丮���� ����ƽ �޽� ��������
UStaticMesh* AJCMInventoryBarBaseActor::GetStaticMeshFromInventory(const int32 InInventoryIndex)
{
	if (!InventoryDataAsset)
	{
		UE_LOG(JCMlog, Error, TEXT("%s : Inventory Data Asset is invalid"), *this->GetName());
		return nullptr;
	}

	if (!InventoryDataAsset->GetStaticMesh(InInventoryIndex))
	{
		UE_LOG(JCMlog, Error, TEXT("%s : Inventory member is invalid"), *this->GetName());
		return nullptr;
	}

	return InventoryDataAsset->GetStaticMesh(InInventoryIndex);
}

// ������ �޽� �ʱ�ȭ
void AJCMInventoryBarBaseActor::InitializeItemStaticMeshPrepertyFromTemplate(UStaticMeshComponent* TargetStaticMeshComponent,
	const int32 InventoryIndex)
{
	if (!TemplateItemMeshComponent)
	{
		UE_LOG(JCMlog, Error, TEXT("TemplateItemStaticMeshComponent is Invalid"));
		return;
	}

	// ����ƽ �޽� ����
	TargetStaticMeshComponent->SetStaticMesh(GetStaticMeshFromInventory(InventoryIndex));

	// ����ƽ �޽� �����ϸ�
	TargetStaticMeshComponent = ScaleStaticMeshToTemplateBounds(TargetStaticMeshComponent, TemplateItemMeshComponent);

	// ������ 
	TargetStaticMeshComponent->SetCastShadow(TemplateItemMeshComponent->CastShadow);
}

// ���� ��ŭ Ŀ���� �޽� ������Ʈ ����
void AJCMInventoryBarBaseActor::CreateSingleCustomMeshComponent(const float BarHeight, const float UnitMeshHeight, int32 SpawnAmount)
{
	// ���� ī��Ʈ ���� üũ, 
	if (SpawnCount >= SpawnAmount)
	{
		ClearSpawnTimerHandle();
	}
	else
	{
		// Ŀ�� ����ƽ �޽� ������Ʈ�� �������� �����ϰ�, �θ� ���Ϳ� ���ϵ��� ����
		UStaticMeshComponent* CoverMeshComponent = NewObject<UStaticMeshComponent>(this);

		// ������ ����ƽ �޽� ������Ʈ�� �������� �����ϰ�, �θ� ���Ϳ� ���ϵ��� ����
		UStaticMeshComponent* ItemMeshComponent = NewObject<UStaticMeshComponent>(this);

		// ���ø����κ��� Ŀ�� �޽�, ��Ƽ����, ������ ����
		InitializeStaticMeshProperty(CoverMeshComponent, CustomStaticMeshTemplateComponent);

		// ���ø����κ��� ������ �޽�, ������ �ʱ�ȭ
		InitializeItemStaticMeshPrepertyFromTemplate(ItemMeshComponent, SpawnCount);

		// ������ �ּ����κ��� �ִ� ���� ������ �ؽ�Ʈ ����
		InitializeTextRenderComponentMaxValue(TextRenderComponentSafeValue, SpawnCount);

		if (bEnablePhysics)
		{
			// ���ø����κ��� ������ ����
			InitializeCustomStaticMeshPhysics(CoverMeshComponent, CustomStaticMeshTemplateComponent);
		}

		// ������ Ŀ���� ����
		ItemMeshComponent->AttachToComponent(CoverMeshComponent, FAttachmentTransformRules::KeepRelativeTransform);

		CoverMeshComponent->AttachToComponent(CustomActorSceneComponent, FAttachmentTransformRules::KeepRelativeTransform);

		// Z�� ���� ������ ����
		if (SpawnPerUnitValue)
		{
			CoverMeshComponent->AddWorldOffset(FVector(0, 0, UnitMeshHeight * SpawnAmount + UnitMeshHeight / 2));
		}
		else
		{
			CoverMeshComponent->AddWorldOffset(FVector(0, 0, BarHeight + UnitMeshHeight * 3));
		}

		// ���忡 ������Ʈ�� ����Ͽ� ���Ϳ� �Բ� �����ǵ��� ���� 
		ItemMeshComponent->RegisterComponent();
		CoverMeshComponent->RegisterComponent();

		// ���� ī��Ʈ
		SpawnCount++;

		// ȸ�� �ִϸ��̼� ���ε�
		InitializeItemMeshRotation(ItemMeshComponent, RotationSpeed);
	}
}

// ����ƽ �޽� ������Ʈ �ϳ��� ����
void AJCMInventoryBarBaseActor::CreateSingleCustomMeshComponent(const float UnitMeshHeight)
{
	// StaticMeshComponent�� �������� �����ϰ�, �θ� ���Ϳ� ���ϵ��� ����
	UStaticMeshComponent* CoverMeshComponent = NewObject<UStaticMeshComponent>(this);

	// ������ ����ƽ �޽� ������Ʈ�� �������� �����ϰ�, �θ� ���Ϳ� ���ϵ��� ����
	UStaticMeshComponent* ItemMeshComponent = NewObject<UStaticMeshComponent>(this);

	// ���ø����κ��� Ŀ�� �޽�, ��Ƽ����, ������ ����
	InitializeStaticMeshProperty(CoverMeshComponent, CustomStaticMeshTemplateComponent);

	// ���ø����κ��� ������ �޽�, ������ �ʱ�ȭ
	InitializeItemStaticMeshPrepertyFromTemplate(ItemMeshComponent, ParentSplineIndex);

	// ������ �ּ����κ��� �ִ� ���� ������ �ؽ�Ʈ ����
	InitializeTextRenderComponentMaxValue(TextRenderComponentSafeValue, SpawnCount);

	if (bEnablePhysics)
	{
		// ���ø����κ��� ������ ����
		InitializeCustomStaticMeshPhysics(CoverMeshComponent, CustomStaticMeshTemplateComponent);
	}

	// ������ Ŀ���� ����
	ItemMeshComponent->AttachToComponent(CoverMeshComponent, FAttachmentTransformRules::KeepRelativeTransform);

	// Ŀ�� ��Ʈ�� ����
	CoverMeshComponent->AttachToComponent(CustomActorSceneComponent, FAttachmentTransformRules::KeepRelativeTransform);

	// Z�� ���� ������ ����
	CoverMeshComponent->AddWorldOffset(FVector(0, 0, UnitMeshHeight / 2));

	CoverMeshComponent->RegisterComponent();
	ItemMeshComponent->RegisterComponent();

	// ȸ�� �ִϸ��̼� ���ε�
	InitializeItemMeshRotation(ItemMeshComponent, RotationSpeed);
}


// ���ø� ����ƽ �޽� ������Ʈ�� �ٿ�� ũ�� ��ŭ Ÿ�� ������Ʈ �����ϸ�
UStaticMeshComponent* AJCMInventoryBarBaseActor::ScaleStaticMeshToTemplateBounds(UStaticMeshComponent* NewMesh, UStaticMeshComponent* TemplateMesh)
{
	if (!NewMesh || !NewMesh->GetStaticMesh() || !TemplateMesh || !TemplateMesh->GetStaticMesh())
	{
		UE_LOG(JCMlog, Warning, TEXT("Invalid Mesh Components"));
		return nullptr;
	}

	// ���ø� �޽� �ٿ��
	FBoxSphereBounds TemplateBounds = TemplateMesh->GetStaticMesh()->GetBounds();

	// �� ������Ʈ�� �ٿ��
	FBoxSphereBounds NewBounds = NewMesh->GetStaticMesh()->GetBounds();

	// ���ø��� Ÿ�� ������Ʈ�� �ִ� ũ�� ��
	float TemplateMaxSize = FMath::Max3(TemplateBounds.BoxExtent.X, TemplateBounds.BoxExtent.Y, TemplateBounds.BoxExtent.Z) * 2.0f;
	float NewMaxSize = FMath::Max3(NewBounds.BoxExtent.X, NewBounds.BoxExtent.Y, NewBounds.BoxExtent.Z) * 2.0f;

	// ũ�� �� ���
	if (NewMaxSize > 0.0f)
	{
		float ScaleFactor = TemplateMaxSize / NewMaxSize;

		// ���ø� ������ ����ŭ �߰� ����
		ScaleFactor *= TemplateMesh->GetRelativeScale3D().X;
		NewMesh->SetRelativeScale3D(FVector(ScaleFactor));
		return NewMesh;
	}
	else
	{
		UE_LOG(JCMlog, Warning, TEXT("New mesh bounds are invalid."));
		return nullptr;
	}
}

void AJCMInventoryBarBaseActor::InitializeItemMeshRotation(UStaticMeshComponent* TargetStaticMeshComponent, 
	const float InRotationSpeed)
{
	// Ÿ�̸Ӹ� ����� ȸ�� �Լ� ȣ��
    FTimerHandle RotationTimerHandle;

	GetWorld()->GetTimerManager().SetTimer(RotationTimerHandle, FTimerDelegate::CreateLambda([TargetStaticMeshComponent, 
		InRotationSpeed]()
		{
			if (TargetStaticMeshComponent)
			{
				// ���� ȸ��
				FRotator CurrentRotation = TargetStaticMeshComponent->GetRelativeRotation();

				// ȸ�� �߰�
				CurrentRotation.Yaw += InRotationSpeed;

				TargetStaticMeshComponent->SetRelativeRotation(CurrentRotation);
			}
		}), 0.01f, true);
}

void AJCMInventoryBarBaseActor::AdjustTextMeshValueOffset(UTextRenderComponent* TargetTextRednerComponent, const float& BarHeight)
{
	TargetTextRednerComponent->AddWorldOffset(FVector(0.f, 0.f, TargetTextRednerComponent->WorldSize / 2
		+ TextRenderComponentOffset_Value + GetCustomMeshUnitHeight()/2));
}

void AJCMInventoryBarBaseActor::AdjustTextMeshValueOffset(UTextRenderComponent* TargetTextRednerComponent, const int32& amount)
{
	// Ŀ���� �޽� ���� ���� : ���� ���� * ���� �����Ϸ�
	float UnitMeshHeight = GetCustomMeshUnitHeight();

	TargetTextRednerComponent->AddWorldOffset(FVector(0.f, 0.f, TargetTextRednerComponent->WorldSize / 2
		+ TextRenderComponentOffset_Value + GetCustomMeshUnitHeight()/2));
}

void AJCMInventoryBarBaseActor::AdjustTextMeshComponentOffsetRoutine(const float InHeight)
{
	AdjustTextMeshValueOffset(TextRenderComponentValue, InHeight); // �ؽ�Ʈ ���� ������Ʈ ������ ����
	AdjustTextMeshOffset(TextRenderComponentLabel);
	AdjustTextMeshOffset(TextRenderComponentSafeValue);
}

