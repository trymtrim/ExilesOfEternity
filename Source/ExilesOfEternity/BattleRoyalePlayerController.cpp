// Copyright Sharp Raccoon 2019.

#include "BattleRoyalePlayerController.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/PlayerStart.h"
#include "GameFramework/Character.h"
#include "Engine/World.h"
#include "BattleRoyaleGameState.h"
#include "UnrealNetwork.h"

void ABattleRoyalePlayerController::BeginPlay ()
{
	//Enable mouse cursor
	ShowMouseCursor (true);

	//Set input mode to UI only
	FInputModeUIOnly uiInputMode;
	SetInputMode (uiInputMode);
}

void ABattleRoyalePlayerController::Tick (float DeltaTime)
{
	if (GetWorld ()->IsServer ())
	{
		//Get game state
		ABattleRoyaleGameState* gameState = Cast <ABattleRoyaleGameState> (GetWorld ()->GetGameState ());

		//If game has not started
		if (!gameState->GetGameStarted ())
		{
			float worldTimeSeconds = GetWorld ()->GetGameState ()->GetServerWorldTimeSeconds ();

			//If starting zone is not chosen before time runs out, automatically choose starting zone
			if (!GetStartingZoneChosen () && gameState->GetGameStartTime () - worldTimeSeconds < 1.0f)
			{
				//Select first available starting zone
				for (int i = 1; i < 21; i++)
				{
					if (gameState->GetStartingZoneAvailable (i))
						SelectStartingZone (i);
				}
			}
		}
		else if (!_gameStarted && gameState->GetGameStarted ())
			RegisterGameStart ();
	}
}

void ABattleRoyalePlayerController::SelectStartingZone_Implementation (int zoneIndex)
{
	//If starting zone already is set, return
	if (_chosenStartingZone != 0)
		return;

	//Get game state
	ABattleRoyaleGameState* gameState = Cast <ABattleRoyaleGameState> (GetWorld ()->GetGameState ());

	//If the given starting zone is unavailable, return
	if (!gameState->GetStartingZoneAvailable (zoneIndex))
		return;

	//Get all start locations
	TArray <AActor*> playerStarts;
	UGameplayStatics::GetAllActorsOfClass (GetWorld (), APlayerStart::StaticClass (), playerStarts);

	//Find the correct player start based on zone index and set the player's location to that location
	for (int i = 0; i < playerStarts.Num (); i++)
	{
		if (Cast <APlayerStart> (playerStarts [i])->PlayerStartTag == FName (*FString::FromInt (zoneIndex)))
			GetCharacter ()->SetActorLocation (playerStarts [i]->GetActorLocation ());
	}
	
	//Set starting zone taken
	gameState->SetStartingZoneTaken (zoneIndex);
	//Set chosen starting zone
	_chosenStartingZone = zoneIndex;
}

bool ABattleRoyalePlayerController::SelectStartingZone_Validate (int zoneIndex)
{
	return true;
}

void ABattleRoyalePlayerController::RegisterGameStart ()
{
	_gameStarted = true;

	ClientRegisterGameStart ();
}

void ABattleRoyalePlayerController::ClientRegisterGameStart_Implementation ()
{
	ShowMouseCursor (false);
	_gameStarted = true;
}

bool ABattleRoyalePlayerController::GetStartingZoneChosen ()
{
	if (_chosenStartingZone == 0)
		return false;

	return true;
}

int ABattleRoyalePlayerController::GetChosenStartingZone ()
{
	return _chosenStartingZone;
}

void ABattleRoyalePlayerController::GetLifetimeReplicatedProps (TArray <FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps (OutLifetimeProps);

	DOREPLIFETIME (ABattleRoyalePlayerController, _chosenStartingZone);
}
