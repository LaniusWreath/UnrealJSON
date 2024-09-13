// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "DataManageGameInstance.h"
#include "Data3DActor.generated.h"

UCLASS(Blueprintable)
class TEST_API AData3DActor : public AActor
{
	GENERATED_BODY()
	

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UFUNCTION(BlueprintCallable, Category = "Graph")
	virtual void ConstructGraph();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	// Data Getter Function
	UPROPERTY(BlueprintReadWrite, Category = "Data")
	TMap<FString, FString> ParsedJSONData;

private:
	
	// Referenced DataManager
	UPROPERTY()
	UDataManager* DataManager;
	UFUNCTION()
	void InitilizeDataManager();

	// Get Data From DataManager
	UFUNCTION()
	void CacheJSONDatas();
};
