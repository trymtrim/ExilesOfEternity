// Copyright Sharp Raccoon 2019.

#include "BattleRoyalePlayerState.h"
#include "BattleRoyaleGameState.h"
#include "Engine/World.h"
#include "UnrealNetwork.h"
#include "BattleRoyalePlayerController.h"
#include "CharacterBase.h"

ABattleRoyalePlayerState::ABattleRoyalePlayerState ()
{
	NetUpdateFrequency = 30.0f;
}

void ABattleRoyalePlayerState::Tick (float DeltaTime)
{
	PrimaryActorTick.bCanEverTick = true;

	//If redeem kill timer is currently running, update it
	if (_currentRedeemKillTime > 0.0f)
		UpdateRedeemKillTimer (DeltaTime);
}

void ABattleRoyalePlayerState::OnKill ()
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
		_permanentDead = true;
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
		_permanentDead = true;

		//Die
		Cast <ACharacterBase> (GetPawn ())->Die ();
	}
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

void ABattleRoyalePlayerState::GetLifetimeReplicatedProps (TArray <FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps (OutLifetimeProps);

	DOREPLIFETIME_CONDITION (ABattleRoyalePlayerState, _currentRedeemKillTime, COND_OwnerOnly);
	DOREPLIFETIME_CONDITION (ABattleRoyalePlayerState, _requiredRedeemKills, COND_OwnerOnly);
	DOREPLIFETIME_CONDITION (ABattleRoyalePlayerState, _permanentDead, COND_OwnerOnly);
}
