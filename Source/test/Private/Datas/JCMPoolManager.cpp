// Fill out your copyright notice in the Description page of Project Settings.


#include "Datas/JCMPoolManager.h"

void UJCMPoolManager::Initialize(AActor* Owner, int32 Size)
{
	OwnerActor = Owner;
    PoolSize = Size;

    // PoolSize��ŭ StaticMeshComponent ���� �� Ǯ�� �߰�
    for (int32 i = 0; i < PoolSize; ++i)
    {
        UStaticMeshComponent* NewComponent = NewObject<UStaticMeshComponent>(OwnerActor);
        if (NewComponent)
        {
            NewComponent->AttachToComponent(OwnerActor->GetRootComponent(), FAttachmentTransformRules::KeepRelativeTransform);
            NewComponent->SetVisibility(false);
            NewComponent->SetCollisionEnabled(ECollisionEnabled::NoCollision);
            Pool.Add(NewComponent);
        }
    }
}
