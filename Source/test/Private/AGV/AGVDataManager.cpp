// Fill out your copyright notice in the Description page of Project Settings.

#include "AGV/AGVDataManager.h"
#include "JsonUtilities.h"
#include "AGV/AGVLog.h"
#include "AGV/AGVDataContainer.h"
#include "SFCommon/SFCHttpManager.h"
#include "SFCommon/SFCWebSocketManager.h"


//--------------------------------- Data Management --------------------------------------------


// Static ��ü
UAGVDataManager* UAGVDataManager::AGVDataManagerInstance = nullptr;

// ������ �Ŵ��� �ν��Ͻ� �ʱ�ȭ
const UAGVDataManager* UAGVDataManager::GetAGVDataManager()
{
	if (!AGVDataManagerInstance)
	{
		AGVDataManagerInstance = NewObject<UAGVDataManager>();
		AGVDataManagerInstance->AddToRoot();  // GC ����
	}
	return AGVDataManagerInstance;
}

// Json ������Ʈ ���޹޾� ����ü�� ��ȯ
FAGVData UAGVDataManager::JsonObjectToAGVStruct(const TSharedPtr<FJsonObject> OriginObject)
{
	FAGVData NewData = FAGVData();
	if (!OriginObject)
	{
		UE_LOG(AGVlog, Error, TEXT("AGVDataManager : Creating CreateDataContainer Failed"));
	}

	if (!FJsonObjectConverter::JsonObjectToUStruct(OriginObject.ToSharedRef(), &NewData))
	{
		UE_LOG(AGVlog, Error, TEXT("Failed to convert JSON to struct"));
	}

	return NewData;
}

// ����ü �Է¹޾� �����̳� �ν��Ͻ�
UAGVDataContainer* UAGVDataManager::CreateDataContainer(UObject* Outer, const FAGVData& InputData)
{
	UAGVDataContainer* NewContainer = NewObject<UAGVDataContainer>(Outer);
	if (NewContainer)
	{
		NewContainer->SetAGVData(InputData);
	}
	return NewContainer;
}

// �� AGV ������ �����̳� ����
UAGVDataContainer* UAGVDataManager::CreateEmptyDataContainer(UObject* Outer)
{
	UAGVDataContainer* NewContainer = NewObject<UAGVDataContainer>(Outer);

	return NewContainer;
}


//--------------------------------- HTTP --------------------------------------------

// Public Instance 
USFCHttpManager* UAGVDataManager::InitializeHttpHandler()
{
	if (!HttpHandler)
	{
		HttpHandler = NewObject<USFCHttpManager>(this);
		HttpHandler->OnParsedJsonObjectPtrReady.BindUObject(this, &UAGVDataManager::SetHTTPJsonObject);
	}
	return HttpHandler;
}

USFCHttpManager* UAGVDataManager::GetHttpHandler()
{
	if (!HttpHandler)
	{
		return InitializeHttpHandler();
	}
	else
	{
		return HttpHandler;
	}
}

// ���� ������ �����̳� ��ü�� ������ ���� �Է��Ͽ� ������Ʈ
UAGVDataContainer* UAGVDataManager::UpdateContainerwithHTTPData(UAGVDataContainer* TargetContainer)
{
	if (!TargetContainer)
	{
		UE_LOG(AGVlog, Error, TEXT("Faild to find target AGVDataContainer"));
		return nullptr;
	}
	if (!TempHTTPJsonObject)
	{
		UE_LOG(AGVlog, Warning, TEXT("No JsonObject left"));
	}
	TargetContainer->SetAGVData(JsonObjectToAGVStruct(TempHTTPJsonObject));

	return TargetContainer;
}

void UAGVDataManager::RequestJsonObject(const FString& URL)
{
	if(!HttpHandler)
	{
		UE_LOG(AGVlog, Warning, TEXT("Failed to find HttpHandler"));
		InitializeHttpHandler();
	}
	HttpHandler->MakeGetRequest(URL, false);
}

void UAGVDataManager::SetHTTPJsonObject(const TSharedPtr<FJsonObject> OriginJsonObject)
{
	if (!OriginJsonObject)
	{
		UE_LOG(AGVlog, Error, TEXT("Failed to find Origin Json Object"));
		return;
	}
	TempHTTPJsonObject = OriginJsonObject;
}


//--------------------------------- WebSocket --------------------------------------------

USFCWebSocketManager* UAGVDataManager::InitializeWebSocketHandler()
{
	if (!WebSocketHandler)
	{
		WebSocketHandler = NewObject<USFCWebSocketManager>(this);
		WebSocketHandler->OnMessageReceivedDelegate.BindUObject(this, &UAGVDataManager::SetWebSocketMessage);
	}
	return WebSocketHandler;
}

// �޽��� �޾� ���ڿ� �״�� ����(�ǽð����� json ��ȯ ��� �ϸ� ������� �ʹ� �߻�)
void UAGVDataManager::SetWebSocketMessage(const FString& Message)
{
	UE_LOG(LogTemp, Log, TEXT("DataManager: Handling WebSocket message: %s"), *Message);

	ReceivedMessage = Message;
}

USFCWebSocketManager* UAGVDataManager::GetWebSocketHandler()
{
	if (!WebSocketHandler)
	{
		return InitializeWebSocketHandler();
	}
	else
	{
		return WebSocketHandler;
	}
}

void UAGVDataManager::ConnectWebSocketServer(const FString& ServerAddress)
{
	if (!WebSocketHandler)
	{
		UE_LOG(AGVlog, Warning, TEXT("Failed to find HttpHandler"));
		InitializeHttpHandler();
	}
	WebSocketHandler->Connect(ServerAddress);
}