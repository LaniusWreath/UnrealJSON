// Fill out your copyright notice in the Description page of Project Settings.


#include "SFCommon/SFCHttpManager.h"

void USFCHttpManager::MakeGetRequest(const FString& Url, const bool GetResultWithFString)
{
	FHttpModule* Http = &FHttpModule::Get();
	if (!Http) return;

	TSharedRef<IHttpRequest, ESPMode::ThreadSafe> Request = Http->CreateRequest();
	Request->SetURL(Url);
	Request->SetVerb(TEXT("GET"));

	// ���� �Լ� ��������Ʈ ���ε�
	if (GetResultWithFString)
	{
		Request->OnProcessRequestComplete().BindUObject(this, &USFCHttpManager::OnResponseReceived);
	}
	else
	{
		Request->OnProcessRequestComplete().BindUObject(this, &USFCHttpManager::OnResponseReceivedWithPtr);
	}

	// ��û ����
	Request->ProcessRequest();
}

void USFCHttpManager::OnResponseReceived(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bWasSuccessful)
{


}

void USFCHttpManager::OnResponseReceivedWithPtr(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bWasSuccessful)
{
	if (bWasSuccessful && Response.IsValid())
	{
		// ���� ������ Ȯ��
		ResultResponseString = Response->GetContentAsString();
		//UE_LOG(JCMlog, Log, TEXT("Response: %s"), *ResultResponseString);

		TSharedPtr<FJsonObject> JsonData;

		// JSON �Ľ�
		TSharedRef<TJsonReader<>> Reader = TJsonReaderFactory<>::Create(ResultResponseString);
		if (FJsonSerializer::Deserialize(Reader, JsonData) && JsonData.IsValid())
		{
			// �Ľ� ���� �Լ� ȣ��
			ParsedJsonData = JsonData;
		}
	}
}
