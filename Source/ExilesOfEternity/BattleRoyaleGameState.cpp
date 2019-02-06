// Copyright Sharp Raccoon 2019.

#include "BattleRoyaleGameState.h"
#include "Kismet/GameplayStatics.h"
#include "Engine/World.h"
#include "GameFramework/PlayerStart.h"
#include "GameFramework/Character.h"
#include "UnrealNetwork.h"
#include "BattleRoyalePlayerState.h"
#include "BattleRoyalePlayerController.h"

void ABattleRoyaleGameState::SetStartingZoneTaken (int zoneIndex)
{
	_takenStartingZones.Add (zoneIndex);
}

void ABattleRoyaleGameState::StartGame ()
{
	_gameStarted = true;
	_stage = 1;
}

int ABattleRoyaleGameState::GetGameStartTime ()
{
	return _gameStartTime;
}

bool ABattleRoyaleGameState::GetStartingZoneAvailable (int zoneIndex)
{
	return !_takenStartingZones.Contains (zoneIndex);
}

bool ABattleRoyaleGameState::GetGameStarted ()
{
	return _gameStarted;
}

int ABattleRoyaleGameState::GetStage ()
{
	return _stage;
}

void ABattleRoyaleGameState::GetLifetimeReplicatedProps (TArray <FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps (OutLifetimeProps);

	DOREPLIFETIME (ABattleRoyaleGameState, _takenStartingZones);
	DOREPLIFETIME (ABattleRoyaleGameState, _gameStarted);
}
