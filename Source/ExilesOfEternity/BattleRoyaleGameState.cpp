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
#include "ExilesOfEternityGameModeBase.h"

void ABattleRoyaleGameState::LoadGameInfo (UGameStageInfo* gameStageInfo, UPlayerProgressionInfo* playerProgressionInfo)
{
	_gameStageInfo = gameStageInfo;
	_playerProgressionInfo = playerProgressionInfo;
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

	//Set game start time
	_startTime = GetServerWorldTimeSeconds ();

	//Assign player count
	_playerCount = PlayerArray.Num ();

	//Initialize scoreboard player stats
	for (int i = 0; i < PlayerArray.Num (); i++)
		AddScoreboardPlayerStats (Cast <ABattleRoyalePlayerState> (PlayerArray [i]));

	//Start timer for current stage and start next stage when timer is finished
	FTimerHandle startNextStageTimerHandle;
	GetWorld ()->GetTimerManager ().SetTimer (startNextStageTimerHandle, this, &ABattleRoyaleGameState::StartNextStage, _gameStageInfo->StageDurations [_stage - 1], false);

	//Initialize player states
	for (int i = 0; i < PlayerArray.Num (); i++)
		Cast <ABattleRoyalePlayerState> (PlayerArray [i])->Initialize ();
}

void ABattleRoyaleGameState::StartNextStage ()
{
	//If game has ended, return
	if (_gameEnded)
		return;

	//Change to next stage
	_stage++;

	//Set current stage start time
	_currentStageStartTime = GetServerWorldTimeSeconds ();

	//Tell play area circle to start shrinking
	_playAreaCircle->StartShrinking (_stage);

	//Broadcast new stage to all clients
	BroadcastStartNextStage (_stage);

	//If current stage is not the last stage, start timer for current stage and start next stage when timer is finished
	if (_stage != 4)
	{
		FTimerHandle startNextStageTimerHandle;
		GetWorld ()->GetTimerManager ().SetTimer (startNextStageTimerHandle, this, &ABattleRoyaleGameState::StartNextStage, _gameStageInfo->StageDurations [_stage - 1], false);
	}
}

void ABattleRoyaleGameState::BroadcastStartNextStage_Implementation (int stage)
{
	OnNewStageBP.Broadcast (stage);
}

void ABattleRoyaleGameState::ReportShrinkingStopped ()
{
	BroadcastShrinkingStopped ();
}

void ABattleRoyaleGameState::BroadcastShrinkingStopped_Implementation ()
{
	OnShrinkingStoppedBP.Broadcast ();
}

void ABattleRoyaleGameState::ReportPermanentDeath (ABattleRoyalePlayerState* playerState)
{
	//Add player to list of permanent dead players
	_permanentDeadPlayers.Add (playerState);

	//Check if a player has won
	CheckForVictory (nullptr);
}

void ABattleRoyaleGameState::CheckForVictory (ABattleRoyalePlayerState* disconnectedPlayer)
{
	//If game is finished, return
	if (_gameEnded)
		return;

	//If list of players contains the player who just disconnected, remove that player from the list
	if (disconnectedPlayer != nullptr)
	{
		if (PlayerArray.Contains (disconnectedPlayer))
			PlayerArray.Remove (disconnectedPlayer);
	}

	//Check if there is only one player alive left
	int alivePlayers = 0;
	ABattleRoyalePlayerState* winningPlayer = nullptr;

	for (int i = 0; i < PlayerArray.Num (); i++)
	{
		if (!_permanentDeadPlayers.Contains (PlayerArray [i]))
		{
			alivePlayers++;
			winningPlayer = Cast <ABattleRoyalePlayerState> (PlayerArray [i]);
		}
	}

	//If there is only one player still alive, make that player victorious
	if (alivePlayers == 1)
	{
		winningPlayer->MakeVictorious ();

		//Set input mode to UI only
		Cast <ABattleRoyalePlayerController> (winningPlayer->GetPawn ()->GetController ())->SetInputUIOnly ();

		//End game
		EndGame ();
	}
}

void ABattleRoyaleGameState::EndGame ()
{
	_gameEnded = true;

	Cast <AExilesOfEternityGameModeBase> (GetWorld ()->GetAuthGameMode ())->EndGame ();
}

UPlayerProgressionInfo* ABattleRoyaleGameState::GetPlayerProgressionInfo ()
{
	return _playerProgressionInfo;
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

float ABattleRoyaleGameState::GetStartTime ()
{
	return _startTime;
}

int ABattleRoyaleGameState::GetStage ()
{
	return _stage;
}

int ABattleRoyaleGameState::GetStageDuration (int stageIndex)
{
	return _gameStageInfo->StageDurations [stageIndex - 1];
}

FVector ABattleRoyaleGameState::GetCircleLocation ()
{
	return _playAreaCircle->GetActorLocation ();
}

FVector ABattleRoyaleGameState::GetCircleScale ()
{
	return _playAreaCircle->GetActorScale3D ();
}

FVector ABattleRoyaleGameState::GetNextCircleLocation ()
{
	return _playAreaCircle->GetEndLocation ();
}

FVector ABattleRoyaleGameState::GetNextCircleScale ()
{
	return FVector (_gameStageInfo->StageDiameters [_stage - 1], _gameStageInfo->StageDiameters [_stage - 1], 0.0f);
}

float ABattleRoyaleGameState::GetStageTimeLeft ()
{
	if (_stage == 4)
		return 0.0f;

	float stageTimeLeft = _startTime + 1;

	for (int i = 0; i < _stage; i++)
		stageTimeLeft += _gameStageInfo->StageDurations [i];

	stageTimeLeft -= GetServerWorldTimeSeconds ();

	return stageTimeLeft;
}

float ABattleRoyaleGameState::GetCircleClosingTimeLeft ()
{
	if (_stage < 2)
		return 0.0f;

	float closingTimeLeft = _gameStageInfo->StageDiameters [_stage - 2] - _gameStageInfo->StageDiameters [_stage - 1];
	closingTimeLeft /= _gameStageInfo->ShrinkSpeed;

	closingTimeLeft += _currentStageStartTime;
	closingTimeLeft -= GetServerWorldTimeSeconds ();

	return closingTimeLeft;
}

float ABattleRoyaleGameState::GetCircleClosingTimeLeftPercentage ()
{
	if (_stage < 2)
		return 0.0f;

	float closingTime = _gameStageInfo->StageDiameters [_stage - 2] - _gameStageInfo->StageDiameters [_stage - 1];
	closingTime /= _gameStageInfo->ShrinkSpeed;

	float closingTimeLeftPercentage = 1 - (GetCircleClosingTimeLeft () / closingTime);

	return closingTimeLeftPercentage;
}

float ABattleRoyaleGameState::GetRedeemKillTime ()
{
	return _gameStageInfo->RedeemKillTime;
}

bool ABattleRoyaleGameState::GetSpawnPositionInsidePlayArea (int spawnIndex)
{
	//Get all spawn locations
	TArray <AActor*> playerStarts;
	UGameplayStatics::GetAllActorsOfClass (GetWorld (), APlayerStart::StaticClass (), playerStarts);

	AActor* spawnLocation = nullptr;

	//Find the correct spawn locatioin based on spawn index
	for (int i = 0; i < playerStarts.Num (); i++)
	{
		if (Cast <APlayerStart> (playerStarts [i])->PlayerStartTag == FName (*FString::FromInt (spawnIndex)))
			spawnLocation = playerStarts [i];
	}

	//If the actor is inside of the play area, return true
	if (_playAreaCircle->GetActorInsidePlayArea (spawnLocation))
		return true;

	//Otherwise, return false
	return false;
}
 
TArray <ABattleRoyalePlayerState*> ABattleRoyaleGameState::GetPermanentDeadPlayers ()
{
	return _permanentDeadPlayers;
}

int ABattleRoyaleGameState::GetPlayerCount ()
{
	return _playerCount;
}

//Scoreboard

void ABattleRoyaleGameState::AddScoreboardPlayerStats (ABattleRoyalePlayerState* playerState)
{
	FPlayerStats playerStats;

	playerStats.Name = playerState->GetNickname ();
	playerStats.Kills = 0;
	playerStats.Deaths = 0;

	playerStats.playerState = playerState;

	_scoreboardPlayerStats.Add (playerStats);
}

void ABattleRoyaleGameState::UpdateScoreboardPlayerStats (ABattleRoyalePlayerState* playerState)
{
	for (int i = 0; i < _scoreboardPlayerStats.Num (); i++)
	{
		if (_scoreboardPlayerStats [i].playerState == playerState)
		{
			_scoreboardPlayerStats [i].Kills = playerState->GetKills ();
			_scoreboardPlayerStats [i].Deaths = playerState->GetDeaths ();

			break;
		}
	}
}

TArray <FPlayerStats> ABattleRoyaleGameState::GetAllPlayerStats ()
{
	return _scoreboardPlayerStats;
}

void ABattleRoyaleGameState::GetLifetimeReplicatedProps (TArray <FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps (OutLifetimeProps);

	DOREPLIFETIME (ABattleRoyaleGameState, _takenStartingZones);
	DOREPLIFETIME (ABattleRoyaleGameState, _gameStarted);
	DOREPLIFETIME (ABattleRoyaleGameState, _startTime);
	DOREPLIFETIME (ABattleRoyaleGameState, _stage);
	DOREPLIFETIME (ABattleRoyaleGameState, _scoreboardPlayerStats);
	DOREPLIFETIME (ABattleRoyaleGameState, _currentStageStartTime);
}
