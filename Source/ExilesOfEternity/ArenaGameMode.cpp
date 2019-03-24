// Copyright Sharp Raccoon 2019.

#include "ArenaGameMode.h"
#include "CharacterBase.h"
#include "ArenaPlayerController.h"
#include "ArenaPlayerState.h"
#include "ArenaGameState.h"

AArenaGameMode::AArenaGameMode ()
{
	//Set default player controller class
	PlayerControllerClass = AArenaPlayerController::StaticClass ();

	//Set default game state class
	GameStateClass = AArenaGameState::StaticClass ();

	//Set default player state class
	PlayerStateClass = AArenaPlayerState::StaticClass ();

	//Enable tick
	PrimaryActorTick.bCanEverTick = true;
}

void AArenaGameMode::Tick (float DeltaTime)
{
	if (!_gameStarted)
		CheckForGameStart ();
}

void AArenaGameMode::CheckForGameStart ()
{
	//Get game state
	AArenaGameState* gameState = Cast <AArenaGameState> (GameState);

	//If start timer is finished, start game
	if (gameState->GetGameStartTime () - gameState->GetServerWorldTimeSeconds () <= 0.0f)
	{
		//Automatically choose spawn location for every player that has not chosen yet
		for (FConstPlayerControllerIterator Iterator = GetWorld ()->GetPlayerControllerIterator (); Iterator; ++Iterator)
		{
			AArenaPlayerController* playerController = Cast <AArenaPlayerController> (Cast <APlayerController> (*Iterator));

			//Register for player that the game is starting
			playerController->RegisterGameStart ();
		}

		gameState->StartGame ();

		//Set game to started
		_gameStarted = true;
	}
}

void AArenaGameMode::ReportDeath (ACharacterBase* characterController)
{
	Cast <AArenaGameState> (GameState)->CheckForVictory ();

	//Respawn character
	/*FTimerHandle respawnTimerHandle;
	GetWorld ()->GetTimerManager ().SetTimer (respawnTimerHandle, characterController, &ACharacterBase::ClientHandleRespawn, timeBeforeRespawn, false);*/
}
