// Fill out your copyright notice in the Description page of Project Settings.

// �������Ʈ�� ���� ��, ������ ���� �ν��Ͻ� �ؾ���.
// �ν��Ͻ����� �ϳ��� json url�� response json ��ü�� ������.

#include "Datas/HTTPRequestManager.h"

void UHTTPRequestManager::MakeGetRequest(const FString& Url)
{
    FHttpModule* Http = &FHttpModule::Get();
    if (!Http) return;

    TSharedRef<IHttpRequest, ESPMode::ThreadSafe> Request = Http->CreateRequest();
    Request->SetURL(Url);
    Request->SetVerb(TEXT("GET"));

    // �⺻ ��� ���� (�ּ� ����)
    /*Request->SetHeader(TEXT("User-Agent"), TEXT("UnrealEngine/5.4"));
    Request->SetHeader(TEXT("Content-Type"), TEXT("application/json"));
    Request->SetHeader(TEXT("Accept"), TEXT("application/json"));*/

    // ������ �ʿ��� ��� �߰� ���� (�ּ� ����)
    //Request->SetHeader(TEXT("Authorization"), TEXT("Bearer YOUR_ACCESS_TOKEN"));

    // ���� ó�� �Լ� ��������Ʈ ���ε�
    Request->OnProcessRequestComplete().BindUObject(this, &UHTTPRequestManager::OnResponseReceived);

    // ��û ����
    Request->ProcessRequest();
}

void UHTTPRequestManager::OnResponseReceived(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bWasSuccessful)
{
    if (bWasSuccessful && Response.IsValid())
    {
        // ���� ������ Ȯ��
        FString ResponseString = Response->GetContentAsString();
        UE_LOG(LogTemp, Log, TEXT("Response: %s"), *ResponseString);

        // JSON �Ľ�
        TSharedRef<TJsonReader<>> Reader = TJsonReaderFactory<>::Create(ResponseString);

        if (FJsonSerializer::Deserialize(Reader, JsonData)&& JsonData.IsValid())
        {
            // �븮�� ȣ��
            OnJsonDataReady.ExecuteIfBound(JsonData);

            // FString���� ���� (�����Ϳ��� ���� ��� ȣ�� �����ϰԲ�)
            TSharedRef<TJsonWriter<>> Writer = TJsonWriterFactory<>::Create(&ResultResponseString);
            UE_LOG(LogTemp, Log, TEXT("HTTPRequestManager : Json Response Saved"));
        }
        else
        {
            UE_LOG(LogTemp, Error, TEXT("Failed to parse JSON."));
        }
    }
    else
    {
        UE_LOG(LogTemp, Error, TEXT("HTTP Request failed."));
    }
}
