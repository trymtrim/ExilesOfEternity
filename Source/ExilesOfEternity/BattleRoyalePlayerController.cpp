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
#include "Runtime/Engine/Public/TimerManager.h"
#include "BattleRoyaleGameMode.h"

void ABattleRoyalePlayerController::BeginPlay ()
{
	//Set input mode to UI only
	SetInputUIOnly ();

	//If game has already started, remove player from game
	if (!GetWorld ()->IsServer ())
	{
		if (Cast <ABattleRoyaleGameState> (GetWorld ()->GetGameState ())->GetGameStarted ())
			UGameplayStatics::OpenLevel (GetWorld (), "LobbyLevel");
	}
}

void ABattleRoyalePlayerController::Tick (float DeltaTime)
{
	//If redeem kill timer is activated, update player state
	if (_redeemTimerActivated)
		_playerState->Tick (DeltaTime);
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

	//Set player ready
	Cast <ABattleRoyaleGameMode> (GetWorld ()->GetAuthGameMode ())->AddReadyPlayer ();

	//Set player health
	Cast <ACharacterBase> (GetCharacter ())->SetHealth (gameState->GetPlayerProgressionInfo ()->HealthPerLevel [0]);
	//Set basic spell damage
	Cast <ACharacterBase> (GetCharacter ())->SetBasicSpellDamage (gameState->GetPlayerProgressionInfo ()->BasicSpellDamagePerLevel [0]);
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

void ABattleRoyalePlayerController::SelectPlayerSpawnPosition_Implementation (int zoneIndex)
{
	_selectedPlayerSpawnPosition = zoneIndex;
}

bool ABattleRoyalePlayerController::SelectPlayerSpawnPosition_Validate (int zoneIndex)
{
	return true;
}

void ABattleRoyalePlayerController::SpawnPlayerCharacter (int zoneIndex)
{
	//Get all start locations
	TArray <AActor*> playerStarts;
	UGameplayStatics::GetAllActorsOfClass (GetWorld (), APlayerStart::StaticClass (), playerStarts);

	//Find the correct player start based on zone index and set the player's location to that location
	for (int i = 0; i < playerStarts.Num (); i++)
	{
		if (Cast <APlayerStart> (playerStarts [i])->PlayerStartTag == FName (*FString::FromInt (zoneIndex)))
		{
			//Reset character
			FTimerDelegate resetDelegate = FTimerDelegate::CreateUObject (this, &ABattleRoyalePlayerController::ResetPlayerCharacter, playerStarts [i]->GetActorLocation ());
			FTimerHandle resetTimerHandle;
			GetWorld ()->GetTimerManager ().SetTimer (resetTimerHandle, resetDelegate, 1.0f, false);
		}
	}

	//Reset character
	//FTimerHandle resetTimerHandle;
	//GetWorld ()->GetTimerManager ().SetTimer (resetTimerHandle, this, &ABattleRoyalePlayerController::ResetPlayerCharacter, 1.0f, false);

	//Set recent player spawn position to current spawn position
	_recentPlayerSpawnPosition = zoneIndex;

	//Stop respawn timer
	Cast <ABattleRoyalePlayerState> (PlayerState)->StopRepawnTimer ();
}

void ABattleRoyalePlayerController::AutoSpawnPlayerCharacter ()
{
	if (_selectedPlayerSpawnPosition != 0)
	{
		SpawnPlayerCharacter (_selectedPlayerSpawnPosition);
		return;
	}

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

void ABattleRoyalePlayerController::ResetPlayerCharacter (FVector spawnLocation)
{
	GetCharacter ()->SetActorLocation (spawnLocation);

	ShowMouseCursor (false);
	Cast <ACharacterBase> (GetCharacter ())->ResetCharacter ();

	_recentPlayerSpawnPosition = 0;
	_selectedPlayerSpawnPosition = 0;
}

void ABattleRoyalePlayerController::RegisterGameStarting ()
{
	ClientRegisterGameStarting (GetWorld ()->GetGameState ()->GetServerWorldTimeSeconds ());
}

void ABattleRoyalePlayerController::ClientRegisterGameStarting_Implementation (int currentWorldSeconds)
{
	_gameStarting = true;
	_gameStartingWorldSeconds = currentWorldSeconds;
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

void ABattleRoyalePlayerController::ShowLevelUpMessage_Implementation (const FString& message, int level)
{
	OnLevelUpMessageBP.Broadcast (message, level);
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

bool ABattleRoyalePlayerController::GetGameStarting ()
{
	return _gameStarting;
}

int ABattleRoyalePlayerController::GetGameStartingWorldSeconds ()
{
	return _gameStartingWorldSeconds;
}

void ABattleRoyalePlayerController::GetLifetimeReplicatedProps (TArray <FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps (OutLifetimeProps);

	DOREPLIFETIME (ABattleRoyalePlayerController, _chosenStartingZone);
	DOREPLIFETIME (ABattleRoyalePlayerController, _recentPlayerSpawnPosition);
}
