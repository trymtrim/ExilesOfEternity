// Copyright Sharp Raccoon 2019.

#include "ExilesOfEternityGameInstance.h"
#include "Engine/World.h"
#include "WebSocket.h"
#include "WebSocketBlueprintLibrary.h"
#include "WebSockets/LobbyMessageObjects.h"
#include "WebSockets/GameServerMessageObjects.h"
#include "BattleRoyaleGameMode.h"
#include "Kismet/GameplayStatics.h"
#include "Engine/GameViewportClient.h"
#include "ConstructorHelpers.h"

UExilesOfEternityGameInstance::UExilesOfEternityGameInstance ()
{
	//Set default pawn class
	static ConstructorHelpers::FClassFinder <UUserWidget> loadingScreenUI (TEXT ("/Game/UI/Lobby/BattleRoyaleLoadingScreen_UI"));
	if (loadingScreenUI.Class != NULL)
		_loadingScreenUI = loadingScreenUI.Class;
}

void UExilesOfEternityGameInstance::ConnectToMasterServer ()
{
	UE_LOG (LogTemp, Warning, TEXT ("Connecting to master server"));

	_isConnecting = true;

	//Connect to websocket
	_webSocket = UWebSocketBlueprintLibrary::Connect ("ws://" + ipAddress + ":1337"); //35.228.209.156

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
	_isConnecting = false;

	//Close connection
	_webSocket->Close ();
}

void UExilesOfEternityGameInstance::OnConnectionComplete ()
{
	//TODO: Query database from master server for things etc.
	
	OnConnectedBP.Broadcast ();

	_isConnecting = false;
	_isConnected = true;

	UE_LOG (LogTemp, Warning, TEXT ("Connected to master server"));
}

void UExilesOfEternityGameInstance::OnConnectionError (FString error)
{
	UE_LOG (LogTemp, Warning, TEXT ("ERROR"));

	//Disconnect
	DisconnectFromMasterServer ();
}

void UExilesOfEternityGameInstance::OnConnectionClosed ()
{
	_isConnected = false;

	OnDisconnectedBP.Broadcast ();

	UE_LOG (LogTemp, Warning, TEXT ("Disconnected from master server"));
}

void UExilesOfEternityGameInstance::OnMessage (FString message)
{
	UTypeCheck* typeCheck = Cast <UTypeCheck> (UWebSocketBlueprintLibrary::JsonToObject (message, UTypeCheck::StaticClass (), false));
	FString type = typeCheck->type;

	if (type == "JoinGame")
	{
		_loadingScreenWidget = CreateWidget (GetFirstLocalPlayerController (), _loadingScreenUI);
		GetGameViewportClient ()->AddViewportWidgetContent (_loadingScreenWidget->TakeWidget (), 10);

		UJoinGameResponse* joinGameResponse = Cast <UJoinGameResponse> (UWebSocketBlueprintLibrary::JsonToObject (message, UJoinGameResponse::StaticClass (), false));
		OnJoinGameBP.Broadcast (joinGameResponse->ipAddress);
	}
	else if (type == "Refresh")
	{
		URefreshResponse* joinGameResponse = Cast <URefreshResponse> (UWebSocketBlueprintLibrary::JsonToObject (message, URefreshResponse::StaticClass (), false));
		OnRefreshBP.Broadcast (joinGameResponse->gameInstanceNames, joinGameResponse->gameInstanceGameModes);
	}
	else if (type == "JoinLobby")
	{
		UJoinLobbyResponse* joinLobbyResponse = Cast <UJoinLobbyResponse> (UWebSocketBlueprintLibrary::JsonToObject (message, UJoinLobbyResponse::StaticClass (), false));
		OnJoinLobbyBP.Broadcast (joinLobbyResponse->gameName, joinLobbyResponse->gameMode, joinLobbyResponse->playerNames);
	}
	else if (type == "RefreshLobby")
	{
		URefreshLobbyResponse* refreshLobbyResponse = Cast <URefreshLobbyResponse> (UWebSocketBlueprintLibrary::JsonToObject (message, URefreshLobbyResponse::StaticClass (), false));
		OnRefreshLobbyBP.Broadcast (refreshLobbyResponse->gameMode, refreshLobbyResponse->lobbyMaster, refreshLobbyResponse->playerNames, refreshLobbyResponse->playerTeams, refreshLobbyResponse->playerCharacters);
	}

	OnConnectionCheckBP.Broadcast ();
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

void UExilesOfEternityGameInstance::JoinLobby (FString gameName, FString playerName, FString gameMode, FString characterName)
{
	FString message;
	UJoinLobbyRequest* request = NewObject <UJoinLobbyRequest> ();

	request->type = "JoinLobby";
	request->gameName = gameName;
	request->gameMode = gameMode;
	request->characterName = characterName;

	if (playerName == "")
		request->playerName = "Player";
	else
		request->playerName = playerName;

	UWebSocketBlueprintLibrary::ObjectToJson (request, message);
	_webSocket->SendText (message);
}

void UExilesOfEternityGameInstance::ChangeTeam (FString gameName, int teamNumber)
{
	FString message;
	UChangeTeamRequest* request = NewObject <UChangeTeamRequest> ();

	request->type = "ChangeTeam";
	request->gameName = gameName;
	request->teamNumber = teamNumber;

	UWebSocketBlueprintLibrary::ObjectToJson (request, message);
	_webSocket->SendText (message);

	arenaTeamNumber = teamNumber;
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

void UExilesOfEternityGameInstance::StartGame (FString gameName, FString gameMode)
{
	FString message;
	UStartGameRequest* request = NewObject <UStartGameRequest> ();

	request->type = "StartGame";
	request->gameName = gameName;
	request->gameMode = gameMode;

	UWebSocketBlueprintLibrary::ObjectToJson (request, message);
	_webSocket->SendText (message);
}

void UExilesOfEternityGameInstance::OnStart ()
{
	if (GetWorld ()->IsServer ())
	{
		if (UGameplayStatics::GetCurrentLevelName (GetWorld ()) == "GameLevel")
			ConnectToLocalServer ();
	}
}

void UExilesOfEternityGameInstance::ConnectToLocalServer ()
{
	UE_LOG (LogTemp, Warning, TEXT ("Connect to local server"));

	_webSocket = UWebSocketBlueprintLibrary::Connect ("ws://127.0.0.1:1338");

	//Bind on connection complete
	FScriptDelegate onConnectionCompleteDelegate;
	onConnectionCompleteDelegate.BindUFunction (this, "OnLocalConnectionComplete");
	_webSocket->OnConnectComplete.AddUnique (onConnectionCompleteDelegate);

	//Bind on message
	FScriptDelegate onMessageDelegate;
	onMessageDelegate.BindUFunction (this, "OnLocalMessage");
	_webSocket->OnReceiveData.AddUnique (onMessageDelegate);
}

void UExilesOfEternityGameInstance::OnLocalConnectionComplete ()
{
	ObtainGameInfo ();
}

void UExilesOfEternityGameInstance::OnLocalMessage (FString message)
{
	UTypeCheck* typeCheck = Cast <UTypeCheck> (UWebSocketBlueprintLibrary::JsonToObject (message, UTypeCheck::StaticClass (), false));
	FString type = typeCheck->type;

	if (type == "GameInfo")
	{
		UGameInfoResponse* gameInfoResponse = Cast <UGameInfoResponse> (UWebSocketBlueprintLibrary::JsonToObject (message, UGameInfoResponse::StaticClass (), false));
		Cast <ABattleRoyaleGameMode> (GetWorld ()->GetAuthGameMode ())->SetPlayerAmount (gameInfoResponse->playerCount);

		_webSocket->Close ();
	}
}

void UExilesOfEternityGameInstance::ObtainGameInfo ()
{
	FString message;
	UGameInfoRequest* request = NewObject <UGameInfoRequest> ();

	request->type = "GameInfo";
	request->port = GetWorld ()->URL.Port;

	UWebSocketBlueprintLibrary::ObjectToJson (request, message);
	_webSocket->SendText (message);
}

void UExilesOfEternityGameInstance::RemoveLoadingScreen ()
{
	if (_loadingScreenWidget != nullptr)
	{
		GetGameViewportClient ()->RemoveViewportWidgetContent (_loadingScreenWidget->TakeWidget ());
		_loadingScreenWidget = nullptr;
	}
}

bool UExilesOfEternityGameInstance::GetIsConnecting ()
{
	return _isConnecting;
}

bool UExilesOfEternityGameInstance::GetIsConnected ()
{
	return _isConnected;
}

void UExilesOfEternityGameInstance::SetSoundClassVolume (USoundClass* targetSoundClass, float newVolume)
{
	targetSoundClass->Properties.Volume = newVolume;
}

float UExilesOfEternityGameInstance::GetSoundClassVolume (USoundClass* targetSoundClass)
{
	return targetSoundClass->Properties.Volume;
}

void UExilesOfEternityGameInstance::Init ()
{

}

void UExilesOfEternityGameInstance::Shutdown ()
{

}
