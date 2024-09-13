// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "JsonHandler.generated.h"

UCLASS(Blueprintable, BlueprintType)
class TEST_API UJsonHandler : public UObject
{
	GENERATED_BODY()

public:
	/*UFUNCTION(BlueprintCallable, Category = "JSON")*/
	TSharedPtr<FJsonObject> GetJsonObjectData(const FString& FilePath);

	//UPROPERTY(VisibleAnywhere, Category = "Data", meta = (AllowPrivateAccess = "true"))
	//TMap<FString, FString> JsonDataMap;
};
