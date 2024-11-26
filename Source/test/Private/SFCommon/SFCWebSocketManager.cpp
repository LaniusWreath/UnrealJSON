// Fill out your copyright notice in the Description page of Project Settings.

#include "SFCommon/SFCWebSocketManager.h"
#include "SFCommon/SFCLog.h"
#include "WebSocketsModule.h"

// ������ ���� �ʱ�ȭ �� ���� �Լ� ���ε�.
void USFCWebSocketManager::InitWebSocketManager(const FString& ServcerAddress)
{
	if (!FModuleManager::Get().IsModuleLoaded("WebSockets"))
	{
		FModuleManager::Get().LoadModule("WebSockets");
	}

	WebSocketObject = FWebSocketsModule::Get().CreateWebSocket("ws://localhost:8080");

	// �Լ� ���ε�.
	WebSocketObject->OnConnected().AddLambda([]() 
	{
		GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Green, "Successfully Connected");
		UE_LOG(SFClog, Log, TEXT("WebSocket connected to Pixel Streaming Server"));
	});

	WebSocketObject->OnConnectionError().AddLambda([](const FString& ErrorMessage) 
	{
		GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Red, ErrorMessage);
		UE_LOG(SFClog, Error, TEXT("%s"), *ErrorMessage);
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

// ���� ����.
void USFCWebSocketManager::ShutDownWebSocketManager()
{
	if (WebSocketObject->IsConnected())
	{
		WebSocketObject->Close();
	}
}

// ������ �޽��� �۽�.
void USFCWebSocketManager::NotifyServerString(const FString& NotifyString)
{
	if (!WebSocketObject)
	{
		UE_LOG(SFClog, Error, TEXT("WebSocket not initialized"));
		return;
	}

	WebSocketObject->Send(NotifyString);
}


// ����ؾ� �ϴ� ��ü �̸� �ʿ� ����
void USFCWebSocketManager::RegisterObject(const FString& ObjectID, const FMessageDelegate& Callback)
{
	if (!RegisteredObjects.Contains(ObjectID))
	{
		RegisteredObjects.Add(ObjectID, Callback);
		UE_LOG(LogTemp, Log, TEXT("Object %s registered for WebSocket messages."), *ObjectID);
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("Object %s is already registered."), *ObjectID);
	}
}

// ����ߴ� ��ü �̸� ����
void USFCWebSocketManager::UnregisterObject(const FString& ObjectID)
{
	if (RegisteredObjects.Remove(ObjectID) > 0)
	{
		UE_LOG(LogTemp, Log, TEXT("Object %s unregistered."), *ObjectID);
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("Object %s was not registered."), *ObjectID);
	}
}

// �޽��� �޾��� �� ���ε�
void USFCWebSocketManager::OnMessageReceived(const FString& Message)
{
	// Assuming messages are formatted as "ObjectID|Payload"
	FString ObjectID, Payload;
	if (Message.Split(TEXT("|"), &ObjectID, &Payload))
	{
		if (RegisteredObjects.Contains(ObjectID))
		{
			// ������ ������ ��� ��������Ʈ ���� ����
			RegisteredObjects[ObjectID].ExecuteIfBound(Payload);
		}
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("No object registered for ID: %s"), *ObjectID);
		}
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("Invalid message format: %s"), *Message);
	}
}
