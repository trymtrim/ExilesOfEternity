// Copyright Sharp Raccoon 2019.

#include "ArenaPlayerState.h"
#include "ArenaGameState.h"
#include "Engine/World.h"
#include "PlayerControllerBase.h"
#include "ExilesOfEternityGameInstance.h"

void AArenaPlayerState::ClientInitialize (AController* C)
{
	SetArenaTeamNumber (Cast <UExilesOfEternityGameInstance> (GetGameInstance ())->arenaTeamNumber);
}

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

void AArenaPlayerState::SetArenaTeamNumber_Implementation (int teamNumber)
{
	SetTeamNumber (teamNumber);
}

bool AArenaPlayerState::SetArenaTeamNumber_Validate (int teamNumber)
{
	return true;
}
