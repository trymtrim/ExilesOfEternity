// Copyright Sharp Raccoon 2019.

#include "ExilesOfEternityGameInstance.h"
#include "Engine/World.h"
#include "WebSocket.h"
#include "WebSocketBlueprintLibrary.h"
#include "WebSockets/LobbyMessageObjects.h"

void UExilesOfEternityGameInstance::ConnectToMasterServer ()
{
	UE_LOG (LogTemp, Warning, TEXT ("Connect to master server"));

	//Connect to websocket
	_webSocket = UWebSocketBlueprintLibrary::Connect ("ws://127.0.0.1:1337");

	//Bind on connection complete
	FScriptDelegate onConnectionCompleteDelegate;
	onConnectionCompleteDelegate.BindUFunction (this, "OnConnectionComplete");
	_webSocket->OnConnectComplete.AddUnique (onConnectionCompleteDelegate);

	//Bind on connection error
	FScriptDelegate onConnectionErrorDelegate;
	onConnectionErrorDelegate.BindUFunction (this, "OnConnectionError");
	_webSocket->OnConnectError.AddUnique (onConnectionErrorDelegate);

	//Bind on connection closed
	FScriptDelegate onConnectionClosedDelegate;
	onConnectionClosedDelegate.BindUFunction (this, "OnConnectionClosed");
	_webSocket->OnClosed.AddUnique (onConnectionClosedDelegate);

	//Bind on message
	FScriptDelegate onMessageDelegate;
	onMessageDelegate.BindUFunction (this, "OnMessage");
	_webSocket->OnReceiveData.AddUnique (onMessageDelegate);
}

void UExilesOfEternityGameInstance::OnConnectionComplete ()
{
	//TODO: Query database from master server for things etc.

	OnConnectedBP.Broadcast ();
}

void UExilesOfEternityGameInstance::OnConnectionError ()
{

}

void UExilesOfEternityGameInstance::OnConnectionClosed ()
{

}

void UExilesOfEternityGameInstance::OnMessage (FString message)
{
	UTypeCheck* typeCheck = Cast <UTypeCheck> (UWebSocketBlueprintLibrary::JsonToObject (message, UTypeCheck::StaticClass (), false));
	FString type = typeCheck->type;

	if (type == "JoinGame")
	{
		UJoinGameResponse* joinGameResponce = Cast <UJoinGameResponse> (UWebSocketBlueprintLibrary::JsonToObject (message, UJoinGameResponse::StaticClass (), false));
		OnJoinGameBP.Broadcast (joinGameResponce->ipAddress);
	}
	else if (type == "Refresh")
	{
		URefreshResponse* joinGameResponce = Cast <URefreshResponse> (UWebSocketBlueprintLibrary::JsonToObject (message, URefreshResponse::StaticClass (), false));
		OnRefreshBP.Broadcast (joinGameResponce->gameInstanceNames);
	}
}

void UExilesOfEternityGameInstance::CreateGame (FString gameName)
{
	FString message;
	UCreateGameRequest* request = NewObject <UCreateGameRequest> ();

	request->type = "CreateGame";
	request->gameName = gameName;

	UWebSocketBlueprintLibrary::ObjectToJson (request, message);
	_webSocket->SendText (message);
}

void UExilesOfEternityGameInstance::Refresh ()
{
	FString message;
	URefreshRequest* request = NewObject <URefreshRequest> ();

	request->type = "Refresh";

	UWebSocketBlueprintLibrary::ObjectToJson (request, message);
	_webSocket->SendText (message);
}

void UExilesOfEternityGameInstance::JoinGame (FString gameName)
{
	FString message;
	UJoinGameRequest* request = NewObject <UJoinGameRequest> ();

	request->type = "JoinGame";
	request->gameName = gameName;

	UWebSocketBlueprintLibrary::ObjectToJson (request, message);
	_webSocket->SendText (message);
}

/*void UExilesOfEternityGameInstance::ConnectToLocalServer ()
{
	UE_LOG (LogTemp, Warning, TEXT ("Connect to local server"));

	_webSocket = UWebSocketBlueprintLibrary::Connect ("ws://127.0.0.1:1338");
}*/

void UExilesOfEternityGameInstance::Init ()
{

}

void UExilesOfEternityGameInstance::OnStart ()
{

}

void UExilesOfEternityGameInstance::Shutdown ()
{

}
