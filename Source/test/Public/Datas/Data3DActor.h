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

	// 현재 차트 타입
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	FString CurrentChartType;

	// 에디터 상에서 결정할 차트 제너레이터 컴포넌트의 클래스 
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ChartSettings", meta = (DisplayPriority = 0))
	TSubclassOf<UChartGenerator> ChartGeneratorComponentClass;

	UFUNCTION()
	void InitilizeDataManager();


protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	// 데이터 인스턴스 저장 가상함수 
	UFUNCTION(BlueprintCallable, Category = "Chart")
	virtual void SetDataClassInstance() PURE_VIRTUAL(UDataFetcherBase::FetchData, ;);

	// 가상함수
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

	// ABarBaseActor BP서 참조, Data3DActorBar에서 할당
	UPROPERTY(EditAnywhere, BlueprintReadWrite ,Category = "Chart")
	TSubclassOf<ABarBaseActor> BarBaseActorBPClass;

};