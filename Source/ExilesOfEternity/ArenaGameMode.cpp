// Copyright Sharp Raccoon 2019.

#include "ArenaGameMode.h"
#include "CharacterBase.h"
#include "ArenaPlayerController.h"
#include "ArenaPlayerState.h"

AArenaGameMode::AArenaGameMode ()
{
	//Set default player controller class
	PlayerControllerClass = AArenaPlayerController::StaticClass ();

	//Set default game state class
	//GameStateClass = ABattleRoyaleGameState::StaticClass ();

	//Set default player state class
	PlayerStateClass = AArenaPlayerState::StaticClass ();
}

/*void AArenaGameMode::BeginPlay ()
{

}*/

void AArenaGameMode::StartGame ()
{

}

/*void AArenaGameMode::ReportDeath (ACharacterBase* characterController)
{

}*/
