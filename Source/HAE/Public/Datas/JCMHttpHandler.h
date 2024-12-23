// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "SFCommon/SFCHttpManager.h"
#include "JCMHttpHandler.generated.h"

/**
 * 
 */

UCLASS(Blueprintable)
class HAE_API UJCMHttpHandler : public USFCHttpManager
{
	GENERATED_BODY()

public:
	// Call HTTP Get Request
	UFUNCTION(BlueprintCallable, meta = (AdvancedDisplay = "GetResultWithFString"), Category = "JCM")
	virtual void MakeGetRequest(const FString& Url, const bool GetResultWithFString = true) override;

	UFUNCTION(BlueprintCallable, meta = (AdvancedDisplay = "GetResultWithFString"), Category = "JCM")
	virtual void MakeGetRequestWithHeader(const FString& Url, const TMap<FString, FString>& Headers,
		const TMap<FString, FString>& Parameters, const bool GetResultWithFString = true) override;

	UFUNCTION(BlueprintCallable, Category = "JCM")
	static TMap<FString, FString> ParseJsonStringToMap(const FString& JsonString);

	UFUNCTION(BlueprintCallable, Category = "JCM")
	static TArray<float> ParseStringToFloatArray(const FString& ArrayString);

	UFUNCTION(BlueprintCallable, Category = "JCM")
	static TArray<FString> ParseStringToStringArray(const FString& ArrayString);

	UFUNCTION(BlueprintCallable, Category = "JCM")
	static UJCMHttpHandler* CreateHttpHandlerInstance(UObject* Outer);

public:

private:

	// HTTP Processing
	void OnResponseReceivedWithString(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bWasSuccessful) override;

	void OnResponseReceivedWithPtr(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bWasSuccessful) override;

	void ExecuteCustomParseFucntion(TSharedPtr<FJsonObject> OriginJsonObject) override;

};