// Copyright Sharp Raccoon 2019.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "LobbyMessageObjects.generated.h"

UCLASS()
class EXILESOFETERNITY_API ULobbyMessageObjects : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()
};

UCLASS(BlueprintType, Blueprintable)
class UTypeCheck : public UObject
{
	GENERATED_BODY ()

public:
	UTypeCheck ();
	UPROPERTY (EditAnywhere, BlueprintReadWrite, meta = (ExposeOnSpawn = true), Category = LobbyMessageObjects)
	FString type;
};

//REQUEST OBJECTS

UCLASS(BlueprintType, Blueprintable)
class UCreateGameRequest : public UObject
{
	GENERATED_BODY ()

public:
	UCreateGameRequest ();
	UPROPERTY (EditAnywhere, BlueprintReadWrite, meta = (ExposeOnSpawn = true), Category = MessageObjects)
	FString type;
	UPROPERTY (EditAnywhere, BlueprintReadWrite, meta = (ExposeOnSpawn = true), Category = MessageObjects)
	FString gameName;
	UPROPERTY (EditAnywhere, BlueprintReadWrite, meta = (ExposeOnSpawn = true), Category = MessageObjects)
	FString gameMode;
};

UCLASS(BlueprintType, Blueprintable)
class UJoinGameRequest : public UObject
{
	GENERATED_BODY ()

public:
	UJoinGameRequest ();
	UPROPERTY (EditAnywhere, BlueprintReadWrite, meta = (ExposeOnSpawn = true), Category = MessageObjects)
	FString type;
	UPROPERTY (EditAnywhere, BlueprintReadWrite, meta = (ExposeOnSpawn = true), Category = MessageObjects)
	FString gameName;
};

UCLASS(BlueprintType, Blueprintable)
class URefreshRequest : public UObject
{
	GENERATED_BODY ()

public:
	URefreshRequest ();
	UPROPERTY (EditAnywhere, BlueprintReadWrite, meta = (ExposeOnSpawn = true), Category = MessageObjects)
	FString type;
};

UCLASS(BlueprintType, Blueprintable)
class UJoinLobbyRequest : public UObject
{
	GENERATED_BODY ()

public:
	UJoinLobbyRequest ();
	UPROPERTY (EditAnywhere, BlueprintReadWrite, meta = (ExposeOnSpawn = true), Category = MessageObjects)
	FString type;
	UPROPERTY (EditAnywhere, BlueprintReadWrite, meta = (ExposeOnSpawn = true), Category = MessageObjects)
	FString gameName;
	UPROPERTY (EditAnywhere, BlueprintReadWrite, meta = (ExposeOnSpawn = true), Category = MessageObjects)
	FString playerName;
};

UCLASS(BlueprintType, Blueprintable)
class ULeaveLobbyRequest : public UObject
{
	GENERATED_BODY ()

public:
	ULeaveLobbyRequest ();
	UPROPERTY (EditAnywhere, BlueprintReadWrite, meta = (ExposeOnSpawn = true), Category = MessageObjects)
	FString type;
	UPROPERTY (EditAnywhere, BlueprintReadWrite, meta = (ExposeOnSpawn = true), Category = MessageObjects)
	FString gameName;
	UPROPERTY (EditAnywhere, BlueprintReadWrite, meta = (ExposeOnSpawn = true), Category = MessageObjects)
	FString playerName;
};

UCLASS(BlueprintType, Blueprintable)
class UStartGameRequest : public UObject
{
	GENERATED_BODY ()

public:
	UStartGameRequest ();
	UPROPERTY (EditAnywhere, BlueprintReadWrite, meta = (ExposeOnSpawn = true), Category = MessageObjects)
	FString type;
	UPROPERTY (EditAnywhere, BlueprintReadWrite, meta = (ExposeOnSpawn = true), Category = MessageObjects)
	FString gameName;
};

//RESPONSE OBJECTS

UCLASS(BlueprintType, Blueprintable)
class UJoinGameResponse : public UObject
{
	GENERATED_BODY ()

public:
	UJoinGameResponse ();
	UPROPERTY (EditAnywhere, BlueprintReadWrite, meta = (ExposeOnSpawn = true), Category = MessageObjects)
	FString type;
	UPROPERTY (EditAnywhere, BlueprintReadWrite, meta = (ExposeOnSpawn = true), Category = MessageObjects)
	FString ipAddress;
};

UCLASS(BlueprintType, Blueprintable)
class URefreshResponse : public UObject
{
	GENERATED_BODY ()

public:
	URefreshResponse ();
	UPROPERTY (EditAnywhere, BlueprintReadWrite, meta = (ExposeOnSpawn = true), Category = MessageObjects)
	FString type;
	UPROPERTY (EditAnywhere, BlueprintReadWrite, meta = (ExposeOnSpawn = true), Category = MessageObjects)
	TArray <FString> gameInstanceNames;
};

UCLASS(BlueprintType, Blueprintable)
class UJoinLobbyResponse : public UObject
{
	GENERATED_BODY ()

public:
	UJoinLobbyResponse ();
	UPROPERTY (EditAnywhere, BlueprintReadWrite, meta = (ExposeOnSpawn = true), Category = MessageObjects)
	FString type;
	UPROPERTY (EditAnywhere, BlueprintReadWrite, meta = (ExposeOnSpawn = true), Category = MessageObjects)
	FString gameName;
	UPROPERTY (EditAnywhere, BlueprintReadWrite, meta = (ExposeOnSpawn = true), Category = MessageObjects)
	TArray <FString> playerNames;
};

UCLASS(BlueprintType, Blueprintable)
class URefreshLobbyResponse : public UObject
{
	GENERATED_BODY ()

public:
	URefreshLobbyResponse ();
	UPROPERTY (EditAnywhere, BlueprintReadWrite, meta = (ExposeOnSpawn = true), Category = MessageObjects)
	FString type;
	UPROPERTY (EditAnywhere, BlueprintReadWrite, meta = (ExposeOnSpawn = true), Category = MessageObjects)
	TArray <FString> playerNames;
};
