// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "DataManageGameInstance.h"
#include "BarBaseActor.h"
#include "Data3DActor.generated.h"

UCLASS(Blueprintable)
class TEST_API AData3DActor : public AActor
{
	GENERATED_BODY()
	

public:	
	AData3DActor();

	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UFUNCTION(BlueprintCallable, Category = "Data")
	void GetDataFromDataManager();

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Component")
	UStaticMeshComponent* BaseMesh;

	// ABarBaseActor BP¼­ ÂüÁ¶
	UPROPERTY(EditAnywhere, Category = "BarGraph")
	TSubclassOf<ABarBaseActor> BarBase;


	UFUNCTION(BlueprintCallable, Category = "Chart")
	void GenerateShapeChart();

	UFUNCTION(BlueprintCallable, Category = "Chart")
	void ClearChildrenActors();


protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;


private:
	// Referenced DataManager
	UPROPERTY()
	UDataManager* DataManagerPtr;
	UFUNCTION(BlueprintCallable)
	void InitilizeDataManager();

};
