// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "AGVDataTypes.h"
#include "AGVDataManager.generated.h"

class UAGVDataContainer;
/**
 * 
 */
UCLASS()
class TEST_API UAGVDataManager : public UObject
{
	GENERATED_BODY()

public:

	// ����ü �Է¹޾� �����̳� �ν��Ͻ�
	UFUNCTION(BlueprintCallable, Category = "AGV")
	static UAGVDataContainer* CreateDataContainer(UObject* Outer, const FAGVData& InputData);
	
	// Json ��ü �Է¹޾� �����̳� �ν��Ͻ�
	static FAGVData CreateDataContainer(const TSharedPtr<FJsonObject> OriginObject);
};
