// Copyright Sharp Raccoon 2019.

#include "BattleRoyaleGameState.h"
#include "Kismet/GameplayStatics.h"
#include "Engine/World.h"
#include "GameFramework/PlayerStart.h"
#include "GameFramework/Character.h"
#include "UnrealNetwork.h"
#include "BattleRoyalePlayerState.h"
#include "BattleRoyalePlayerController.h"
#include "ConstructorHelpers.h"
#include "PlayAreaCircle.h"

void ABattleRoyaleGameState::LoadGameStageInfo (UGameStageInfo* gameStageInfo)
{
	_gameStageInfo = gameStageInfo;
}

void ABattleRoyaleGameState::BeginPlay ()
{
	//Get play area circle from level
	TArray <AActor*> playAreaCircles;
	UGameplayStatics::GetAllActorsOfClass (GetWorld (), APlayAreaCircle::StaticClass (), playAreaCircles);
	_playAreaCircle = Cast <APlayAreaCircle> (playAreaCircles [0]);
}

void ABattleRoyaleGameState::SetStartingZoneTaken (int zoneIndex)
{
	_takenStartingZones.Add (zoneIndex);
}

void ABattleRoyaleGameState::StartGame ()
{
	//Initialize game start
	_gameStarted = true;
	_stage = 1;

	//Start timer for current stage and start next stage when timer is finished
	FTimerHandle startNextStageTimerHandle;
	GetWorld ()->GetTimerManager ().SetTimer (startNextStageTimerHandle, this, &ABattleRoyaleGameState::StartNextStage, _gameStageInfo->StageDurations [_stage - 1], false);
}

void ABattleRoyaleGameState::StartNextStage ()
{
	//Change to next stage
	_stage++;

	//Tell play area circle to start shrinking
	_playAreaCircle->StartShrinking (_stage);

	//If current stage is not the last stage, start timer for current stage and start next stage when timer is finished
	if (_stage != 4)
	{
		FTimerHandle startNextStageTimerHandle;
		GetWorld ()->GetTimerManager ().SetTimer (startNextStageTimerHandle, this, &ABattleRoyaleGameState::StartNextStage, _gameStageInfo->StageDurations [_stage - 1], false);
	}
}

int ABattleRoyaleGameState::GetGameStartTime ()
{
	return _gameStageInfo->GameStartTime;
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
