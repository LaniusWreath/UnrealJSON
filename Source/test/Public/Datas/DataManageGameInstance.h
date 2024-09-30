// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "DataManageGameInstance.generated.h"

class UDataManager;

UCLASS()
class TEST_API UDataManageGameInstance : public UGameInstance
{
	GENERATED_BODY()

private:
	UPROPERTY()
	UDataManager* DataManager;

public:

	virtual void Init() override;

	UFUNCTION(BlueprintCallable, Category = "Data Management")
	UDataManager* GetDataManager() const;


};
