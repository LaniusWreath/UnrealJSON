// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Datas/Data3DActor.h"
#include "BarDataActor.generated.h"

/**
 * 
 */
UCLASS(Blueprintable)
class TEST_API ABarDataActor : public AData3DActor
{
	GENERATED_BODY()

public:
	virtual void ConstructGraph() override;

};

