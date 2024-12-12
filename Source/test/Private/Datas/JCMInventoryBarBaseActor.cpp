// Fill out your copyright notice in the Description page of Project Settings.


#include "Datas/JCMInventoryBarBaseActor.h"
#include "Datas/JCMLog.h"
#include "Datas/StaticMeshInventory.h"


AJCMInventoryBarBaseActor::AJCMInventoryBarBaseActor()
{
	TemplateItemMeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("InventoryItemStaticMeshComponent"));
	TemplateItemMeshComponent->SetupAttachment(CustomStaticMeshTemplateComponent);
}

// �Է¹��� ����ƽ �޽� ������Ʈ�� �κ��丮 �޽� ����
void AJCMInventoryBarBaseActor::InitializeStaticMeshPropertyFromInventory(UStaticMeshComponent* TargetStaticMeshComponent, const int32 InInventoryIndex)
{
	UStaticMesh* TemplateStaticMesh = GetStaticMeshFromInventory(InInventoryIndex);

	if (!TemplateStaticMesh)
	{
		UE_LOG(JCMlog, Error, TEXT("%s : Invalid StaticMesh"), *this->GetActorLabel());
		return;
	}

	TargetStaticMeshComponent->SetStaticMesh(TemplateStaticMesh);
}

// �κ��丮���� ����ƽ �޽� ��������
UStaticMesh* AJCMInventoryBarBaseActor::GetStaticMeshFromInventory(const int32 InInventoryIndex)
{
	if (!InventoryDataAsset)
	{
		UE_LOG(JCMlog, Error, TEXT("%s : Inventory Data Asset is invalid"), *this->GetActorLabel());
		return nullptr;
	}

	if (!InventoryDataAsset->GetStaticMesh(InInventoryIndex))
	{
		UE_LOG(JCMlog, Error, TEXT("%s : Inventory member is invalid"), *this->GetActorLabel());
		return nullptr;
	}

	return InventoryDataAsset->GetStaticMesh(InInventoryIndex);
}

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

	// ����ƽ �޽� ������Ʈ �����ϸ�
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
		NewMesh->SetRelativeScale3D(FVector(ScaleFactor));
		return NewMesh;
	}
	else
	{
		UE_LOG(JCMlog, Warning, TEXT("New mesh bounds are invalid."));
		return nullptr;
	}
}