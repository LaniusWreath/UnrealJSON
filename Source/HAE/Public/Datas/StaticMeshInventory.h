// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "StaticMeshInventory.generated.h"

/**
 * 
 */
UCLASS(BlueprintType)
class HAE_API UStaticMeshInventory : public UDataAsset
{
	GENERATED_BODY()
	

private:
	// StaticMesh Array
	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"), Category = "JCM")
	TArray<UStaticMesh*> StaticMeshArray;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"), Category = "JCM")
	TArray<int32> SafeAmountArray;

public:
	TArray<UStaticMesh*> GetStaticMeshArray();

	// Get Mesh
	UStaticMesh* GetStaticMesh(const int32 InventoryIndex);

	// Get Amount
	const int32 GetAmount(const int32 InventoryIndex);
	
};