﻿// Fill out your copyright notice in the Description page of Project Settings.

// 블루프린트로 뽑은 뒤, 레벨에 따로 인스턴싱 해야함.
// 인스턴스마다 하나의 json url과 response json 객체를 저장함.

#include "Datas/JCMHttpHandler.h"
#include "Datas/JCMLog.h"

// Object / String 공통 Request 함수
void UJCMHttpHandler::MakeGetRequest(const FString& Url, const bool GetResultWithFString)
{
	Super::MakeGetRequest(Url, GetResultWithFString);
}

// String Response 함수
void UJCMHttpHandler::OnResponseReceivedWithString(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bWasSuccessful)
{
	Super::OnResponseReceivedWithString(Request, Response, bWasSuccessful);
}

// Object Response 함수
void UJCMHttpHandler::OnResponseReceivedWithPtr(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bWasSuccessful)
{
	if (bWasSuccessful && Response.IsValid())
	{
		// 응답 데이터 확인
		ResultResponseString = Response->GetContentAsString();
		//UE_LOG(JCMlog, Log, TEXT("Response: %s"), *ResultResponseString);

		TSharedPtr<FJsonObject> JsonData;

		// JSON 파싱
		TSharedRef<TJsonReader<>> Reader = TJsonReaderFactory<>::Create(ResultResponseString);
		if (FJsonSerializer::Deserialize(Reader, JsonData) && JsonData.IsValid())
		{
			// 파싱 실행 함수 호출
			ParsedJsonData = JsonData;
			ExecuteCustomParseFucntion(JsonData);
		}
		else
		{
			UE_LOG(JCMlog, Error, TEXT("%s : Failed to parse JSON."), *this->GetName());
		}
	}
	else
	{
		UE_LOG(JCMlog, Error, TEXT("%s : HTTP Request failed."), *this->GetName());
	}
}

// 데이터 형식에 맞는 커스텀 파싱 함수 호출. (Object Response 전용)
void UJCMHttpHandler::ExecuteCustomParseFucntion(TSharedPtr<FJsonObject> OriginJsonObject)
{
	ParsedJsonData = ParseRequestBody(OriginJsonObject);
	if (ParsedJsonData)
	{
		OnParsedJsonObjectPtrReady.Execute(ParsedJsonData);
		OnRequestingProcessEvent.Broadcast();
	}
}

// 실제 파싱 함수
TSharedPtr<FJsonObject> UJCMHttpHandler::ParseRequestBody(TSharedPtr<FJsonObject> RequestBody)
{
	return Super::ParseRequestBody(RequestBody);
}

// JsonString을 Map으로 반환하는 함수
TMap<FString, FString> UJCMHttpHandler::ParseJsonStringToMap(const FString& JsonString)
{
	return Super::ParseJsonStringToMap(JsonString);
}

// "[1,2,3,4,5]" 형태 문자열 배열로 추출
TArray<float> UJCMHttpHandler::ParseStringToFloatArray(const FString& ArrayString)
{
	return Super::ParseStringToFloatArray(ArrayString);
}

// "[a,b,c,d,e]" 형태 문자열 배열로 추출
TArray<FString> UJCMHttpHandler::ParseStringToStringArray(const FString& ArrayString)
{
	return Super::ParseStringToStringArray(ArrayString);
}
