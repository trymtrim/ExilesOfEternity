// Copyright Sharp Raccoon 2019.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "GameServerMessageObjects.generated.h"

UCLASS()
class EXILESOFETERNITY_API UGameServerMessageObjects : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()
};

//REQUEST OBJECTS

UCLASS(BlueprintType, Blueprintable)
class UGameInfoRequest : public UObject
{
	GENERATED_BODY ()

public:
	UGameInfoRequest ();
	UPROPERTY (EditAnywhere, BlueprintReadWrite, meta = (ExposeOnSpawn = true), Category = MessageObjects)
	FString type;
	UPROPERTY (EditAnywhere, BlueprintReadWrite, meta = (ExposeOnSpawn = true), Category = MessageObjects)
	int port;
};

//RESPONSE OBJECTS

UCLASS(BlueprintType, Blueprintable)
class UGameInfoResponse : public UObject
{
	GENERATED_BODY ()

public:
	UGameInfoResponse ();
	UPROPERTY (EditAnywhere, BlueprintReadWrite, meta = (ExposeOnSpawn = true), Category = MessageObjects)
	FString type;
	UPROPERTY (EditAnywhere, BlueprintReadWrite, meta = (ExposeOnSpawn = true), Category = MessageObjects)
	FString gameName;
	UPROPERTY (EditAnywhere, BlueprintReadWrite, meta = (ExposeOnSpawn = true), Category = MessageObjects)
	int playerCount;
};
