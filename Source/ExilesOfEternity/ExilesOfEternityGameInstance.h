// Copyright Sharp Raccoon 2019.

#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "ExilesOfEternityGameInstance.generated.h"

class UWebSocketBase;

DECLARE_DYNAMIC_MULTICAST_DELEGATE (FOnConnected);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam (FOnJoinGame, FString, ipAddress);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam (FOnRefresh, const TArray <FString>&, gameInstanceNames);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams (FOnJoinLobby, FString, gameName, const TArray <FString>&, playerNames);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam (FOnRefreshLobby, const TArray <FString>&, playerNames);

UCLASS()
class EXILESOFETERNITY_API UExilesOfEternityGameInstance : public UGameInstance
{
	GENERATED_BODY ()

public:
	UFUNCTION (BlueprintCallable)
	void ConnectToMasterServer ();
	//void ConnectToLocalServer ();

	UFUNCTION (BlueprintCallable)
	void CreateGame (FString gameName, FString gameMode);
	UFUNCTION (BlueprintCallable)
	void Refresh ();
	UFUNCTION (BlueprintCallable)
	void JoinGame (FString gameName);
	UFUNCTION (BlueprintCallable)
	void JoinLobby (FString gameName, FString playerName);
	UFUNCTION (BlueprintCallable)
	void LeaveLobby (FString gameName, FString playerName);
	UFUNCTION (BlueprintCallable)
	void StartGame (FString gameName);

	UPROPERTY (BlueprintAssignable)
	FOnConnected OnConnectedBP;
	UPROPERTY (BlueprintAssignable)
	FOnJoinGame OnJoinGameBP;
	UPROPERTY (BlueprintAssignable)
	FOnRefresh OnRefreshBP;
	UPROPERTY (BlueprintAssignable)
	FOnJoinLobby OnJoinLobbyBP;
	UPROPERTY (BlueprintAssignable)
	FOnRefreshLobby OnRefreshLobbyBP;

protected:
	virtual void Init () override;
	virtual void OnStart () override;
	virtual void Shutdown () override;

private:
	UFUNCTION ()
	void OnConnectionComplete ();
	UFUNCTION ()
	void OnConnectionError ();
	UFUNCTION ()
	void OnConnectionClosed ();
	UFUNCTION ()
	void OnMessage (FString message);

	UPROPERTY ()
	UWebSocketBase* _webSocket;
};
