// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "DataTypes.h"
#include "Data3DActor.generated.h"

class UDataManager;
class UDataClasses;
class UChartGenerator;

UCLASS(Abstract)
class TEST_API AData3DActor : public AActor
{
	GENERATED_BODY()

public:	
	AData3DActor();

	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Component")
	UStaticMeshComponent* BaseMesh;

	// ���� ��Ʈ Ÿ��
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	FString CurrentChartType;

	// ������ �󿡼� ������ ��Ʈ ���ʷ����� ������Ʈ�� Ŭ���� 
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ChartSettings", meta = (DisplayPriority = 0))
	TSubclassOf<UChartGenerator> ChartGeneratorComponentClass;

	UFUNCTION()
	void InitilizeDataManager();


protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	// ������ �ν��Ͻ� ���� �����Լ� 
	UFUNCTION(BlueprintCallable, Category = "Chart")
	virtual void SetDataClassInstance() PURE_VIRTUAL(UDataFetcherBase::FetchData, ;);

	// �����Լ�
	UFUNCTION(BlueprintCallable, Category = "Chart")
	virtual void GenerateChartRoutine()  PURE_VIRTUAL(UDataFetcherBase::FetchData, ;);

	// DataManager Reference
	UPROPERTY()
	UDataManager* DataManagerReference;

};

class USplineComponent;
class UArrowComponent;
class UBarGenerator;
class ABarBaseActor;

UCLASS(Blueprintable)
class AData3DActorBar : public AData3DActor
{
	GENERATED_BODY()

private:
	UPROPERTY()
	UBarGenerator* BarGeneratorComponent;

	UPROPERTY()
	UDataClasses* DataClassInstance;

protected:
	virtual void SetDataClassInstance() override;
	virtual void GenerateChartRoutine() override;

public:
	AData3DActorBar();

	// ABarBaseActor BP�� ����, Data3DActorBar���� �Ҵ�
	UPROPERTY(EditAnywhere, BlueprintReadWrite ,Category = "Chart")
	TSubclassOf<ABarBaseActor> BarBaseActorBPClass;

};