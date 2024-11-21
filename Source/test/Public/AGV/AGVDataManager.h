// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "AGVDataTypes.h"
#include "AGVDataManager.generated.h"

class USFCHttpManager;
class UAGVDataContainer;
/**
 * 
 */
UCLASS(Blueprintable)
class TEST_API UAGVDataManager : public UObject
{
	GENERATED_BODY()

public:

	UPROPERTY(BlueprintReadOnly, Category = "AGV")
	USFCHttpManager* HttpHandler;

	UFUNCTION(BlueprintCallable, Category = "AGV")
	USFCHttpManager* InitializeHttpHandler();

	UFUNCTION(BlueprintCallable, Category = "AGV")
	USFCHttpManager* GetHttpHandler();

	UFUNCTION(BlueprintCallable, Category = "AGV")
	static UAGVDataManager* GetAGVDataManager();

	// Instancing AGVDataContainer from Struct input
	UFUNCTION(BlueprintCallable, Category = "AGV")
	static UAGVDataContainer* CreateDataContainer(UObject* Outer, const FAGVData& InputData);

	// Get AGVDataStruct from jsonObject
	static FAGVData JsonObjectToAGVStruct(const TSharedPtr<FJsonObject> OriginObject);

	UFUNCTION(BlueprintCallable, Category = "AGV")
	void RequestJsonObject(const FString& URL);

	UFUNCTION(BlueprintCallable, Category = "AGV")
	UAGVDataContainer* UpdateContainerwithLastData(UAGVDataContainer* TargetContainer);

private:
	// For Delegate Binding Result
	TSharedPtr<FJsonObject> TempJsonObject;
	void SetJsonObject(const TSharedPtr<FJsonObject> OriginObject);

	static UAGVDataManager* AGVDataManagerInstance;
};
