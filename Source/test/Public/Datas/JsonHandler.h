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
	// Json Source Read Function
	TSharedPtr<FJsonObject> GetJsonObjectData(const FString& FilePath);
};
