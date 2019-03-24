// Copyright Sharp Raccoon 2019.

#include "ArenaPlayerState.h"
#include "ArenaGameState.h"
#include "Engine/World.h"
#include "PlayerControllerBase.h"

void AArenaPlayerState::Destroyed ()
{
	if (!GetWorld ()->IsServer ())
		return;

	Cast <AArenaGameState> (GetWorld ()->GetGameState ())->CheckForVictory (this);
}

void AArenaPlayerState::OnKill (APlayerState* playerState)
{
	//Add kill message
	Cast <APlayerControllerBase> (GetPawn ()->GetController ())->AddKillMessage ("You killed " + Cast <APlayerStateBase> (playerState)->GetNickname ());
}
