// Copyright Sharp Raccoon 2019.

#include "ExilesOfEternityGameInstance.h"
#include "Engine/World.h"
#include "WebSocket.h"
#include "WebSocketBlueprintLibrary.h"
#include "WebSockets/LobbyMessageObjects.h"

void UExilesOfEternityGameInstance::ConnectToMasterServer ()
{
	UE_LOG (LogTemp, Warning, TEXT ("Connecting to master server"));

	//Connect to websocket
	_webSocket = UWebSocketBlueprintLibrary::Connect ("ws://35.228.159.15:1337");

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

void UExilesOfEternityGameInstance::DisconnectFromMasterServer ()
{
	//Close connection
	_webSocket->Close ();
}

void UExilesOfEternityGameInstance::OnConnectionComplete ()
{
	//TODO: Query database from master server for things etc.

	OnConnectedBP.Broadcast ();

	UE_LOG (LogTemp, Warning, TEXT ("Connected to master server"));
}

void UExilesOfEternityGameInstance::OnConnectionError ()
{
	//Disconnect
	DisconnectFromMasterServer ();
}

void UExilesOfEternityGameInstance::OnConnectionClosed ()
{
	OnDisconnectedBP.Broadcast ();

	UE_LOG (LogTemp, Warning, TEXT ("Disconnected from master server"));
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
	else if (type == "JoinLobby")
	{
		UJoinLobbyResponse* joinLobbyResponce = Cast <UJoinLobbyResponse> (UWebSocketBlueprintLibrary::JsonToObject (message, UJoinLobbyResponse::StaticClass (), false));
		OnJoinLobbyBP.Broadcast (joinLobbyResponce->gameName, joinLobbyResponce->playerNames);
	}
	else if (type == "RefreshLobby")
	{
		URefreshLobbyResponse* refreshLobbyResponce = Cast <URefreshLobbyResponse> (UWebSocketBlueprintLibrary::JsonToObject (message, URefreshLobbyResponse::StaticClass (), false));
		OnRefreshLobbyBP.Broadcast (refreshLobbyResponce->playerNames);
	}
}

void UExilesOfEternityGameInstance::CreateGame (FString gameName, FString gameMode)
{
	FString message;
	UCreateGameRequest* request = NewObject <UCreateGameRequest> ();

	request->type = "CreateGame";
	request->gameName = gameName;
	request->gameMode = gameMode;

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

void UExilesOfEternityGameInstance::JoinLobby (FString gameName, FString playerName)
{
	FString message;
	UJoinLobbyRequest* request = NewObject <UJoinLobbyRequest> ();

	request->type = "JoinLobby";
	request->gameName = gameName;

	if (playerName == "")
		request->playerName = "Player";
	else
		request->playerName = playerName;

	UWebSocketBlueprintLibrary::ObjectToJson (request, message);
	_webSocket->SendText (message);
}

void UExilesOfEternityGameInstance::LeaveLobby (FString gameName, FString playerName)
{
	FString message;
	ULeaveLobbyRequest* request = NewObject <ULeaveLobbyRequest> ();

	request->type = "LeaveLobby";
	request->gameName = gameName;
	request->playerName = playerName;

	UWebSocketBlueprintLibrary::ObjectToJson (request, message);
	_webSocket->SendText (message);
}

void UExilesOfEternityGameInstance::StartGame (FString gameName)
{
	FString message;
	UStartGameRequest* request = NewObject <UStartGameRequest> ();

	request->type = "StartGame";
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
