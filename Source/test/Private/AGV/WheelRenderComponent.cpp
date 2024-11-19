// Fill out your copyright notice in the Description page of Project Settings.

#include "Components/StaticMeshComponent.h"
#include "AGV/WheelRenderComponent.h"

UWheelRenderComponent::UWheelRenderComponent()
{
    ChassisMeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Chassis"));
    //ChassisMeshComponent->SetupAttachment(this);
}

void UWheelRenderComponent::SetWheelMesh(UStaticMesh* Mesh)
{
    ChassisMeshComponent->SetStaticMesh(Mesh);
    // �߰� ���� ���� : ��Ƽ����, �ݸ��� �� 
}

void UWheelRenderComponent::BeginPlay()
{
}
