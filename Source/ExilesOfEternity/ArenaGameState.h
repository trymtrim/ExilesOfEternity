// Copyright Sharp Raccoon 2019.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameStateBase.h"
#include "ArenaGameState.generated.h"

class AArenaPlayerState;

UCLASS()
class EXILESOFETERNITY_API AArenaGameState : public AGameStateBase
{
	GENERATED_BODY ()

public:
	void StartGame ();

	void CheckForVictory (AArenaPlayerState* disconnectedPlayer = nullptr);
	void FinishRound (int victoryTeamNumber);
	void StartNextRound ();
	void RespawnPlayers ();

	UFUNCTION (BlueprintCallable)
	int GetGameStartTime ();
	UFUNCTION (BlueprintCallable)
	bool GetGameStarted ();
	UFUNCTION (BlueprintCallable)
	bool GetGameOver ();
	UFUNCTION (BlueprintCallable)
	FString GetGameOverText ();

	UPROPERTY (Replicated, BlueprintReadOnly)
	int _blueVictories = 0;
	UPROPERTY (Replicated, BlueprintReadOnly)
	int _redVictories = 0;

private:
	void EndGame (int victoryTeamNumber);

	int _gameStartTime = 30;
	int _winsRequiredToWin = 3;

	int _round = 0;

	UPROPERTY (Replicated)
	bool _gameEnded = false;
	UPROPERTY (Replicated)
	FString _gameOverText;
	UPROPERTY (Replicated)
	bool _gameStarted = false;
};
