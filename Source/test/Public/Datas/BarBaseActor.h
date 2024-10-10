// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "BarBaseActor.generated.h"


class UTextRenderComponent;
class UTimelineComponent;
class UProceduralMeshComponent;

UCLASS()
class TEST_API ABarBaseActor : public AActor
{
	GENERATED_BODY()

private:
	UPROPERTY()
	UProceduralMeshComponent* ProcMeshComponent;

	UPROPERTY()
	UTimelineComponent* BarAnimationTimeline;

	UPROPERTY()
	UTextRenderComponent* TextRenderComponentValue;

	UPROPERTY()
	UTextRenderComponent* TextRenderComponentLabel;

	// TimeLine Animation Binding Function
	UFUNCTION()
	void OnAnimationUpdate(float Value);

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Sets default values for this actor's properties
	ABarBaseActor();

	// Bar Procedural Mesh Material
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Material")
	UMaterialInstance* MeshMaterial;

	// Bar Generate Animation Curve
	UPROPERTY(EditAnywhere, Category = "Chart")
	UCurveFloat* AnimationCurve;

	// Text Mesh Color
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Chart")
	FColor TextColor = FColor::Blue;

	// Bar Mesh Width
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Chart")
	float Width_bar = 10.f;

	// Bar Text Unit Size : Value
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Chart")
	float TextSizeUnit_value = 10;

	// Bar Text Unit Size : Label
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Chart")
	float TextSizeUnit_label = 10;

	// Animation Control Function
	UFUNCTION()
	void PlayBarAnimation();

	// Create Procedural Mesh
	UFUNCTION(BlueprintCallable, Category = "Chart")
	void CreateBarMesh(float BarHeight);

	// Initialize Lbel Text Mesh
	UFUNCTION(BlueprintCallable, Category = " Chart")
	void InitializeTextMeshLabel(const FString& LabelName);

	// Initialize Value Text Mesh
	UFUNCTION(BlueprintCallable, Category = " Chart")
	void InitializeTextMeshValue(const float& FloatValue, const float& BarHeight);
};
