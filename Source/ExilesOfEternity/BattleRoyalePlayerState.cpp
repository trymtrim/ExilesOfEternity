// Copyright Sharp Raccoon 2019.

#include "BattleRoyalePlayerState.h"
#include "BattleRoyaleGameState.h"
#include "Engine/World.h"
#include "UnrealNetwork.h"
#include "BattleRoyalePlayerController.h"
#include "CharacterBase.h"
#include "Kismet/GameplayStatics.h"

ABattleRoyalePlayerState::ABattleRoyalePlayerState ()
{
	NetUpdateFrequency = 30.0f;
}

void ABattleRoyalePlayerState::Initialize ()
{
	//Get player progressions info
	_playerProgressionInfo = Cast <ABattleRoyaleGameState> (GetWorld ()->GetGameState ())->GetPlayerProgressionInfo ();

	//Set needed experience to level up
	_neededExperience = _playerProgressionInfo->ExperienceNeededPerLevel [0];
}

void ABattleRoyalePlayerState::ClientInitialize_Implementation ()
{
	//Get player progressions info
	_playerProgressionInfo = Cast <ABattleRoyaleGameState> (GetWorld ()->GetGameState ())->GetPlayerProgressionInfo ();

	//Set needed experience to level up
	_neededExperience = _playerProgressionInfo->ExperienceNeededPerLevel [0];
}

void ABattleRoyalePlayerState::Tick (float DeltaTime)
{
	PrimaryActorTick.bCanEverTick = true;

	//Update server specific elements
	if (GetWorld ()->IsServer ())
	{
		//If redeem kill timer is currently running and the player is not victorious, update it
		if (_currentRedeemKillTime > 0.0f && !_victorious && !Cast <ACharacterBase> (GetPawn ())->GetDead ())
			UpdateRedeemKillTimer (DeltaTime);
	}
}

void ABattleRoyalePlayerState::GainExperience (int experience)
{
	//If player is already max level, return
	if (_level == _playerProgressionInfo->MaxLevel)
		return;

	//Add experience to current experience
	_currentExperience += experience;

	//If player has reached needed experience, level up
	if (_currentExperience >= _neededExperience)
		LevelUp ();
}

void ABattleRoyalePlayerState::LevelUp ()
{
	//Add one level
	_level++;

	ACharacterBase* characterController = Cast <ACharacterBase> (GetPawn ());
	//Upgrade health
	characterController->SetHealth (_playerProgressionInfo->HealthPerLevel [_level - 1]);
	//Upgrade basic spell damage
	characterController->SetBasicSpellDamage (_playerProgressionInfo->BasicSpellDamagePerLevel [_level - 1]);

	//Add spell upgrade
	characterController->AddSpellUpgrade ();

	//If player reached max level
	if (_level == _playerProgressionInfo->MaxLevel)
	{
		//Set current experience to max
		_currentExperience = _neededExperience;
	}
	else //If player did not reach max level
	{
		//Move the excessive experience to next level
		_currentExperience = _currentExperience - _neededExperience;

		//Set needed experience for next level
		_neededExperience = _playerProgressionInfo->ExperienceNeededPerLevel [_level - 1];

		//If current experience is more than needed experience, level up again
		if (_currentExperience >= _neededExperience)
			LevelUp ();
	}

	//If player reached level 6, unlock ultimate spell
	if (_level == 6)
		characterController->UnlockUltimateSpell ();

	//Set level in character controller
	characterController->level = _level;
}

void ABattleRoyalePlayerState::OnKill (APlayerState* playerState)
{
	//If redeem kill timer is currently running
	if (_currentRedeemKillTime > 0.0f)
	{
		//Remove one required redeem kill
		_requiredRedeemKills--;

		//If there are no required redeem kills left, stop timer
		if (_requiredRedeemKills == 0)
			ActivateRedeemKillTimer (false);
	}

	//Gain experience
	GainExperience (Cast <ABattleRoyalePlayerState> (playerState)->GetLevel () * 40);
}

void ABattleRoyalePlayerState::OnDeath ()
{
	//If character is dead permanently, return
	if (_permanentDead)
		return;

	//Get game state
	ABattleRoyaleGameState* gameState = Cast <ABattleRoyaleGameState> (GetWorld ()->GetGameState ());

	//If the game is in the first stages
	if (gameState->GetStage () < 3)
	{
		//If redeem kill timer is not running
		if (_currentRedeemKillTime <= 0.0f)
		{
			//Start redeem kill timer
			ActivateRedeemKillTimer (true);
			//Set required redeem kills to one
			_requiredRedeemKills = 1;
		}
		else //If redeem kill timer is currently running, add one required redeem kill
			_requiredRedeemKills++;
	}
	else //If the game is in the last stages, die permanenty
		DiePermanently ();
}

void ABattleRoyalePlayerState::ActivateRedeemKillTimer (bool state)
{
	//Get game state
	ABattleRoyaleGameState* gameState = Cast <ABattleRoyaleGameState> (GetWorld ()->GetGameState ());

	//Start or stop timer based on state
	if (state)
		_currentRedeemKillTime = gameState->GetRedeemKillTime ();
	else
		_currentRedeemKillTime = 0.0f;

	//Tell player controller to start timer
	Cast <ABattleRoyalePlayerController> (GetPawn ()->GetController ())->ActivateRedeemTimer (state);
}

void ABattleRoyalePlayerState::UpdateRedeemKillTimer (float deltaTime)
{
	_currentRedeemKillTime -= deltaTime;

	//If redeem kill timer runs out, die permanenty
	if (_currentRedeemKillTime <= 0.0f)
	{
		//Stop timer
		ActivateRedeemKillTimer (false);

		//Permanent death
		DiePermanently ();

		//Die
		Cast <ACharacterBase> (GetPawn ())->Die ();
	}
}

void ABattleRoyalePlayerState::DiePermanently ()
{
	//If player is victorious, return
	if (_victorious)
		return;

	_permanentDead = true;

	//Get game state
	ABattleRoyaleGameState* gameState = Cast <ABattleRoyaleGameState> (GetWorld ()->GetGameState ());

	//Find out how this player placed
	int placement = gameState->GetPlayerCount () - gameState->GetPermanentDeadPlayers ().Num ();

	//Set game over text
	_gameOverText = "You placed " + FString::FromInt (placement) + ". out of " + FString::FromInt (gameState->GetPlayerCount ()) + " players";

	//Update player state in game state
	gameState->ReportPermanentDeath (this);

	//Set input to UI only
	Cast <ABattleRoyalePlayerController> (GetPawn ()->GetController ())->SetInputUIOnly ();
}

void ABattleRoyalePlayerState::StartRespawnTimer (float respawnTime)
{
	//Set respawn time
	_respawnTime = respawnTime;

	//Update respawn timer every second
	FTimerHandle respawnTimerHandle;
	GetWorld ()->GetTimerManager ().SetTimer (respawnTimerHandle, this, &ABattleRoyalePlayerState::UpdateRespawnTimer, 1.0f, false);
}

void ABattleRoyalePlayerState::StopRepawnTimer ()
{
	_respawnTime = -1;
}

void ABattleRoyalePlayerState::UpdateRespawnTimer ()
{
	//If respawn timer is stopped, return
	if (_respawnTime < 0)
		return;

	//If respawn time is not finished, continue updating it
	if (_respawnTime > 1)
	{
		_respawnTime--;

		FTimerHandle respawnTimerHandle;
		GetWorld ()->GetTimerManager ().SetTimer (respawnTimerHandle, this, &ABattleRoyalePlayerState::UpdateRespawnTimer, 1.0f, false);
	}
	else //If respawn time is finished, automatically choose spawn position
		Cast <ABattleRoyalePlayerController> (GetPawn ()->GetController ())->AutoSpawnPlayerCharacter ();
}

void ABattleRoyalePlayerState::MakeVictorious ()
{
	_victorious = true;

	//Set game over text
	_gameOverText = "You are victorious!";
}

int ABattleRoyalePlayerState::GetLevel ()
{
	return _level;
}

int ABattleRoyalePlayerState::GetCurrentExperience ()
{
	return _currentExperience;
}

int ABattleRoyalePlayerState::GetNeededExperience ()
{
	return _neededExperience;
}

float ABattleRoyalePlayerState::GetCurrentRedeemKillTime ()
{
	return _currentRedeemKillTime;
}

int ABattleRoyalePlayerState::GetRequiredRedeemKills ()
{
	return _requiredRedeemKills;
}

bool ABattleRoyalePlayerState::GetPermanentDead ()
{
	return _permanentDead;
}

bool ABattleRoyalePlayerState::GetVictorious ()
{
	return _victorious;
}

int ABattleRoyalePlayerState::GetRespawnTime ()
{
	return _respawnTime;
}

FString ABattleRoyalePlayerState::GetGameOverText ()
{
	return _gameOverText;
}

void ABattleRoyalePlayerState::GetLifetimeReplicatedProps (TArray <FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps (OutLifetimeProps);

	DOREPLIFETIME_CONDITION (ABattleRoyalePlayerState, _currentRedeemKillTime, COND_OwnerOnly);
	DOREPLIFETIME_CONDITION (ABattleRoyalePlayerState, _requiredRedeemKills, COND_OwnerOnly);
	DOREPLIFETIME_CONDITION (ABattleRoyalePlayerState, _victorious, COND_OwnerOnly);
	DOREPLIFETIME_CONDITION (ABattleRoyalePlayerState, _respawnTime, COND_OwnerOnly);
	DOREPLIFETIME_CONDITION (ABattleRoyalePlayerState, _gameOverText, COND_OwnerOnly);
	DOREPLIFETIME_CONDITION (ABattleRoyalePlayerState, _currentExperience, COND_OwnerOnly);
	DOREPLIFETIME_CONDITION (ABattleRoyalePlayerState, _neededExperience, COND_OwnerOnly);

	DOREPLIFETIME (ABattleRoyalePlayerState, _level);
	DOREPLIFETIME (ABattleRoyalePlayerState, _permanentDead);
}
