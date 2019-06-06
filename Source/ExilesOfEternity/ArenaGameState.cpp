// Copyright Sharp Raccoon 2019.

#include "ArenaGameState.h"
#include "UnrealNetwork.h"
#include "ArenaPlayerController.h"
#include "ArenaPlayerState.h"
#include "CharacterBase.h"
#include "Runtime/Engine/Public/TimerManager.h"
#include "ArenaGameMode.h"

void AArenaGameState::StartGame ()
{
	_gameStarted = true;
	
	StartNextRound ();
}

void AArenaGameState::CheckForVictory (AArenaPlayerState* disconnectedPlayer)
{
	//If game has already finished, return
	if (_gameEnded)
		return;

	//If list of players contains the player who just disconnected, remove that player from the list
	if (disconnectedPlayer != nullptr)
	{
		if (PlayerArray.Contains (disconnectedPlayer))
			PlayerArray.Remove (disconnectedPlayer);
	}

	int bluePlayers = 0;
	int deadBluePlayers = 0;

	int redPlayers = 0;
	int deadRedPlayers = 0;

	for (int i = 0; i < PlayerArray.Num (); i++)
	{
		AArenaPlayerState* playerState = Cast <AArenaPlayerState> (PlayerArray [i]);

		if (playerState->GetTeamNumber () == 1)
		{
			bluePlayers++;

			if (Cast <ACharacterBase> (playerState->GetPawn ())->GetDead ())
				deadBluePlayers++;
		}
		else
		{
			redPlayers++;

			if (Cast <ACharacterBase> (playerState->GetPawn ())->GetDead ())
				deadRedPlayers++;
		}
	}

	if (bluePlayers == 0)
		EndGame (2);
	else if (redPlayers == 0)
		EndGame (1);
	else if (bluePlayers == deadBluePlayers && redPlayers != deadRedPlayers)
		FinishRound (2);
	else if (redPlayers == deadRedPlayers && bluePlayers != deadBluePlayers)
		FinishRound (1);
}

void AArenaGameState::FinishRound (int victoryTeamNumber)
{
	FString victoryMessage;

	if (victoryTeamNumber == 1)
	{
		//Add a victory to blue team
		_blueVictories++;

		//Set victory message
		victoryMessage = "Blue team wins the round!";
	}
	else
	{
		//Add a victory to red team
		_redVictories++;

		//Set victory message
		victoryMessage = "Red team wins the round!";
	}

	//If a team has won the entire match, end game
	if (_blueVictories == _winsRequiredToWin)
		EndGame (1);
	else if (_redVictories == _winsRequiredToWin)
		EndGame (2);
	else
	{
		for (int i = 0; i < PlayerArray.Num (); i++)
		{
			//Get player controller
			APlayerControllerBase* playerController = Cast <APlayerControllerBase> (PlayerArray[i]->GetPawn ()->GetController ());

			//Add victory message
			playerController->AddBigMessage (victoryMessage, 3.5f);
		}

		//Start next round after 5 seconds
		FTimerHandle startGameTimerHandle;
		GetWorld ()->GetTimerManager ().SetTimer (startGameTimerHandle, this, &AArenaGameState::StartNextRound, 5.0f, false);
	}
}

void AArenaGameState::StartNextRound ()
{
	_round++;

	for (int i = 0; i < PlayerArray.Num (); i++)
	{
		//Get player controller
		APlayerControllerBase* playerController = Cast <APlayerControllerBase> (PlayerArray [i]->GetPawn ()->GetController ());

		//Add round message
		playerController->AddBigMessage ("Round " + FString::FromInt (_round), 2.0f);
	}

	//Respawn all players
	RespawnPlayers ();
}

void AArenaGameState::RespawnPlayers ()
{
	int blueTeamSpawnIndex = 0;
	int redTeamSpawnIndex = 5;

	for (int i = 0; i < PlayerArray.Num (); i++)
	{
		AArenaPlayerState* playerState = Cast <AArenaPlayerState> (PlayerArray [i]);

		if (playerState->GetTeamNumber () == 1)
		{
			Cast <AArenaPlayerController> (playerState->GetPawn ()->GetController ())->SpawnPlayerCharacter (blueTeamSpawnIndex);
			blueTeamSpawnIndex++;
		}
		else
		{
			Cast <AArenaPlayerController> (playerState->GetPawn ()->GetController ())->SpawnPlayerCharacter (redTeamSpawnIndex);
			redTeamSpawnIndex++;
		}
	}
}

void AArenaGameState::EndGame (int victoryTeamNumber)
{
	_gameEnded = true;

	//Set game over text
	if (victoryTeamNumber == 1)
		_gameOverText = "Blue team is victorious!";
	else
		_gameOverText = "Red team is victorious!";

	//Loop through all palyers
	for (int i = 0; i < PlayerArray.Num (); i++)
	{
		//Set input to UI only
		Cast <AArenaPlayerController> (PlayerArray [i]->GetPawn ()->GetController ())->SetInputUIOnly ();

		//Make player victorious, to avoid actions after game has finished
		Cast <ACharacterBase> (PlayerArray [i]->GetPawn ())->MakeVictorious ();
	}

	Cast <AArenaGameMode> (GetWorld ()->GetAuthGameMode ())->EndGame ();
}

int AArenaGameState::GetGameStartTime ()
{
	return _gameStartTime;
}

bool AArenaGameState::GetGameStarted ()
{
	return _gameStarted;
}

bool AArenaGameState::GetGameOver ()
{
	return _gameEnded;
}

FString AArenaGameState::GetGameOverText ()
{
	return _gameOverText;
}

void AArenaGameState::GetLifetimeReplicatedProps (TArray <FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps (OutLifetimeProps);

	DOREPLIFETIME (AArenaGameState, _gameStarted);
	DOREPLIFETIME (AArenaGameState, _gameEnded);
	DOREPLIFETIME (AArenaGameState, _gameOverText);
	DOREPLIFETIME (AArenaGameState, _blueVictories);
	DOREPLIFETIME (AArenaGameState, _redVictories);
}
