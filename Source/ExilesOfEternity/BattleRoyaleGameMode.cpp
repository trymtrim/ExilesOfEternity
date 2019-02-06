// Copyright Sharp Raccoon 2019.

#include "BattleRoyaleGameMode.h"
#include "BattleRoyaleGameState.h"
#include "BattleRoyalePlayerController.h"
#include "BattleRoyalePlayerState.h"
#include "Engine/World.h"

ABattleRoyaleGameMode::ABattleRoyaleGameMode ()
{
	//Set default player controller class
	PlayerControllerClass = ABattleRoyalePlayerController::StaticClass ();

	//Set default game state class
	GameStateClass = ABattleRoyaleGameState::StaticClass ();

	//Set default player state class
	PlayerStateClass = ABattleRoyalePlayerState::StaticClass ();

	//Enable tick
	PrimaryActorTick.bCanEverTick = true;
}

void ABattleRoyaleGameMode::Tick (float DeltaTime)
{
	if (!_gameStarted)
		CheckForGameStart ();
}

void ABattleRoyaleGameMode::CheckForGameStart ()
{
	//Get game state
	ABattleRoyaleGameState* gameState = Cast <ABattleRoyaleGameState> (GameState);

	//If start timer is finished, start game
	if (gameState->GetGameStartTime () - gameState->GetServerWorldTimeSeconds () <= 0.0f)
	{
		//Automatically choose spawn location for every player that has not chosen yet
		for (FConstPlayerControllerIterator Iterator = GetWorld ()->GetPlayerControllerIterator (); Iterator; ++Iterator)
		{
			ABattleRoyalePlayerController* playerController = Cast <ABattleRoyalePlayerController> (Cast <APlayerController> (*Iterator));

			if (!playerController->GetStartingZoneChosen ())
				playerController->AutoSelectStartingZone ();
		}

		//Officially start game after one second
		FTimerHandle startGameTimerHandle;
		GetWorld ()->GetTimerManager ().SetTimer (startGameTimerHandle, this, &ABattleRoyaleGameMode::StartGame, 1.0f, false);

		//Set game to started
		_gameStarted = true;
	}
}

void ABattleRoyaleGameMode::StartGame ()
{
	//Update game state
	Cast <ABattleRoyaleGameState> (GameState)->StartGame ();

	//Update player controllers
	for (FConstPlayerControllerIterator Iterator = GetWorld ()->GetPlayerControllerIterator (); Iterator; ++Iterator)
	{
		ABattleRoyalePlayerController* playerController = Cast <ABattleRoyalePlayerController> (Cast <APlayerController> (*Iterator));

		playerController->RegisterGameStart ();
	}
}
