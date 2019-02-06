// Copyright Sharp Raccoon 2019.

#include "BattleRoyaleGameMode.h"
#include "BattleRoyaleGameState.h"
#include "BattleRoyalePlayerController.h"
#include "BattleRoyalePlayerState.h"

ABattleRoyaleGameMode::ABattleRoyaleGameMode ()
{
	//Set default player controller class
	PlayerControllerClass = ABattleRoyalePlayerController::StaticClass ();

	//Set default game state class
	GameStateClass = ABattleRoyaleGameState::StaticClass ();

	//Set default player state class
	PlayerStateClass = ABattleRoyalePlayerState::StaticClass ();
}
