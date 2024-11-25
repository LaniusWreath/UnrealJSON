// Fill out your copyright notice in the Description page of Project Settings.

#include "SFCommon/SFCWebSocketManager.h"
#include "SFCommon/SFCLog.h"
#include "WebSocketsModule.h"

// ������ ���� �ʱ�ȭ �� ���� �Լ� ���ε�
void USFCWebSocketManager::InitWebSocketManager(const FString& ServcerAddress)
{
	if (!FModuleManager::Get().IsModuleLoaded("WebSockets"))
	{
		FModuleManager::Get().LoadModule("WebSockets");
	}

	WebSocketObject = FWebSocketsModule::Get().CreateWebSocket("ws://localhost:8080");

	// �Լ� ���ε�
	WebSocketObject->OnConnected().AddLambda([]() 
	{
		GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Green, "Successfully Connected");
		UE_LOG(SFClog, Log, TEXT("WebSocket connected to Pixel Streaming Server"));
	});

	WebSocketObject->OnConnectionError().AddLambda([](const FString& Error) 
	{
		GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Red, Error);
		UE_LOG(SFClog, Error, TEXT("%s"), *Error);
	});

	WebSocketObject->OnClosed().AddLambda([](int32 StatusCode, const FString& Reason, bool bWasCleen)
	{
		GEngine->AddOnScreenDebugMessage(-1, 15.0f, bWasCleen ? FColor::Green : FColor::Red, "Connection Closed" + Reason);
	});

	WebSocketObject->OnMessage().AddLambda([](const FString& Message) {
		GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Cyan, "Received Message: " + Message);

		// ������� �޽��� �ٷ�� �Լ� �ۼ��ؼ� ���ε��� ��,.
	});

	WebSocketObject->OnMessageSent().AddLambda([](const FString& Message) 
	{
		GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Yellow, "Scent Message: " + Message);
	});

	WebSocketObject->Connect();
}

// ���� ����
void USFCWebSocketManager::ShutDownWebSocketManager()
{
	if (WebSocketObject->IsConnected())
	{
		WebSocketObject->Close();
	}
}

// ������ �޽��� �۽�
void USFCWebSocketManager::NotifyServerString(const FString& NotifyString)
{
	if (!WebSocketObject)
	{
		UE_LOG(SFClog, Error, TEXT("WebSocket not initialized"));
		return;
	}

	WebSocketObject->Send(NotifyString);
}
