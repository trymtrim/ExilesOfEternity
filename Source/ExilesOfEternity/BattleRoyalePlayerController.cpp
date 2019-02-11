// Copyright Sharp Raccoon 2019.

#include "BattleRoyalePlayerController.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/PlayerStart.h"
#include "GameFramework/Character.h"
#include "Engine/World.h"
#include "BattleRoyaleGameState.h"
#include "UnrealNetwork.h"
#include "BattleRoyalePlayerState.h"

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
	//If redeem kill timer is activated, update player state
	if (_redeemTimerActivated)
		_playerState->Tick (DeltaTime);

	//GEngine->AddOnScreenDebugMessage (-1, 15.0f, FColor::Yellow, "THIS IS A TEST YO!");
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

void ABattleRoyalePlayerController::AutoSelectStartingZone ()
{
	//Get game state
	ABattleRoyaleGameState* gameState = Cast <ABattleRoyaleGameState> (GetWorld ()->GetGameState ());

	TArray <int> availableStartingZones;

	//Add available starting zones to list of available starting zones
	for (int i = 1; i < 19; i++)
	{
		if (gameState->GetStartingZoneAvailable (i))
			availableStartingZones.Add (i);
	}

	//Select a random starting zone from available starting zones
	int randomIndex = FMath::RandRange (0, availableStartingZones.Num () - 1);
	SelectStartingZone (availableStartingZones [randomIndex]);
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

void ABattleRoyalePlayerController::ActivateRedeemTimer (bool state)
{
	_redeemTimerActivated = state;

	if (_playerState == nullptr)
		_playerState = Cast <ABattleRoyalePlayerState> (PlayerState);
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
