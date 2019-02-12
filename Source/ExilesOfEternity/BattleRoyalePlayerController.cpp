// Copyright Sharp Raccoon 2019.

#include "BattleRoyalePlayerController.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/PlayerStart.h"
#include "GameFramework/Character.h"
#include "Engine/World.h"
#include "BattleRoyaleGameState.h"
#include "UnrealNetwork.h"
#include "BattleRoyalePlayerState.h"
#include "CharacterBase.h"

void ABattleRoyalePlayerController::BeginPlay ()
{
	//Set input mode to UI only
	SetInputUIOnly ();
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

bool ABattleRoyalePlayerController::SelectStartingZone_Validate (int zoneIndex)
{
	return true;
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

void ABattleRoyalePlayerController::SpawnPlayerCharacter_Implementation (int zoneIndex)
{
	//Get all start locations
	TArray <AActor*> playerStarts;
	UGameplayStatics::GetAllActorsOfClass (GetWorld (), APlayerStart::StaticClass (), playerStarts);

	//Find the correct player start based on zone index and set the player's location to that location
	for (int i = 0; i < playerStarts.Num (); i++)
	{
		if (Cast <APlayerStart> (playerStarts [i])->PlayerStartTag == FName (*FString::FromInt (zoneIndex)))
			GetCharacter ()->SetActorLocation (playerStarts [i]->GetActorLocation ());
	}

	//Reset character
	FTimerHandle resetTimerHandle;
	GetWorld ()->GetTimerManager ().SetTimer (resetTimerHandle, this, &ABattleRoyalePlayerController::ResetPlayerCharacter, 1.0f, false);

	//Set recent playeri spawn position to current spawn position
	_recentPlayerSpawnPosition = zoneIndex;

	//Stop respawn timer
	Cast <ABattleRoyalePlayerState> (PlayerState)->StopRepawnTimer ();
}

bool ABattleRoyalePlayerController::SpawnPlayerCharacter_Validate (int zoneIndex)
{
	return true;
}

void ABattleRoyalePlayerController::AutoSpawnPlayerCharacter ()
{
	//Get game state
	ABattleRoyaleGameState* gameState = Cast <ABattleRoyaleGameState> (GetWorld ()->GetGameState ());

	TArray <int> availableSpawnPositions;

	//Add available starting zones to list of available starting zones
	for (int i = 1; i < 19; i++)
	{
		if (gameState->GetSpawnPositionInsidePlayArea (i))
			availableSpawnPositions.Add (i);
	}

	//Select a random spawn position from available spawn positions
	int randomIndex = FMath::RandRange (0, availableSpawnPositions.Num () - 1);
	SpawnPlayerCharacter (availableSpawnPositions [randomIndex]);
}

void ABattleRoyalePlayerController::ResetPlayerCharacter ()
{
	ShowMouseCursor (false);
	Cast <ACharacterBase> (GetCharacter ())->ResetCharacter ();

	_recentPlayerSpawnPosition = 0;
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

int ABattleRoyalePlayerController::GetRecentPlayerSpawnPosition ()
{
	return _recentPlayerSpawnPosition;
}

void ABattleRoyalePlayerController::GetLifetimeReplicatedProps (TArray <FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps (OutLifetimeProps);

	DOREPLIFETIME (ABattleRoyalePlayerController, _chosenStartingZone);
	DOREPLIFETIME (ABattleRoyalePlayerController, _recentPlayerSpawnPosition);
}
