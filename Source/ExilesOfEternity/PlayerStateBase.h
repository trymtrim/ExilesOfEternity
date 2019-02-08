// Copyright Sharp Raccoon 2019.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerState.h"
#include "PlayerStateBase.generated.h"

UCLASS()
class EXILESOFETERNITY_API APlayerStateBase : public APlayerState
{
	GENERATED_BODY ()

public:
	APlayerStateBase ();
	
	void SetNickname (FString name);
	void SetTeamNumber (int number);
	void AddKill ();
	void AddDeath ();

	UFUNCTION (BlueprintCallable)
	FString GetNickname ();
	UFUNCTION (BlueprintCallable)
	int GetTeamNumber ();

	int GetKills ();
	int GetDeaths ();

protected:
	virtual void OnKill ();
	virtual void OnDeath ();

private:
	UPROPERTY (Replicated)
	FString _nickname;
	int _teamNumber;
	UPROPERTY (Replicated)
	int _kills;
	UPROPERTY (Replicated)
	int _deaths;
};
